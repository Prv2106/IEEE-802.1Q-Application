/**
 * @file EthernetApplication.cc
 * @brief Implementation of the periodic TSN traffic source and sink.
 */

#include "EthernetApplication.h"
#include "EthernetDataFrames_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/VlanTag_m.h"
#include "inet/linklayer/common/PcpTag_m.h"
#include "inet/common/ProtocolUtils.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "TSNSocket.h"

Define_Module(EthernetApplication);

/**
 * Initializes the socket callback and output gate, resolves the local and
 * remote MAC addresses, and locates the egress network interface.
 *
 * @param stage Current initialization stage.
 */
void EthernetApplication::initialize(int stage) {
    ApplicationBase::initialize(stage);

    if(stage == INITSTAGE_LOCAL) {
        socket.setCallback(this);
        socket.setOutputGate(gate("socketOut"));

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION+1) {
        const char *localAddressString = par("localAddress");
        if (*localAddressString != '\0') {
            L3Address l3Address;
            L3AddressResolver addressResolver;
            addressResolver.tryResolve(localAddressString, l3Address, L3AddressResolver::ADDR_MAC);
            if (l3Address.getType() == L3Address::MAC)
                localAddress = l3Address.toMac();
            else
                localAddress = MacAddress(localAddressString);
        }
        const char *remoteAddressString = par("remoteAddress");
        if (*remoteAddressString != '\0') {
            L3Address l3Address;
            L3AddressResolver addressResolver;
            addressResolver.tryResolve(remoteAddressString, l3Address, L3AddressResolver::ADDR_MAC);
            if (l3Address.getType() == L3Address::MAC)
                remoteAddress = l3Address.toMac();
            else
                remoteAddress = MacAddress(remoteAddressString);
        }

        auto interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        networkInterface = interfaceTable->findInterfaceByName(par("netInterface"));
        if (networkInterface == nullptr)
            throw cRuntimeError("Cannot find network interface");
        if (!localAddress.isUnspecified())
            networkInterface->addMulticastMacAddress(localAddress);
    }
}

/**
 * Dispatches self-messages, frame-generation timers, inter-arrival
 * timers, and messages received from the socket.
 *
 * The first generation is triggered by StartTimer. GenTimer then triggers
 * subsequent generations, while InterTimer resumes transmission after the
 * configured inter-arrival time.
 *
 * @param msg Message to process.
 */
void EthernetApplication::handleMessageWhenUp(cMessage *msg) {
    if(msg->isSelfMessage()) {
        if(strcmp(msg->getName(), "StartTimer") ==  0) {
            generateFrames();
            delete msg;
            cMessage *tim = new cMessage("GenTimer");
            scheduleAt(par("period")+simTime(), tim);
            return;
        } else if(strcmp(msg->getName(), "GenTimer") ==  0) {
            generateFrames();
            scheduleAt(par("period")+simTime(), msg);
            return;
        } else if(strcmp(msg->getName(), "InterTimer") ==  0) {
            delete msg;
            transmitFrames();
            return;
        }
    }

    if (socket.belongsToSocket(msg)) {
        socket.processMessage(msg);
        return;
    }



    delete msg;
}

/**
 * Generates one burst of frames and enqueues them for transmission.
 *
 * The payload is split into frames whose size does not exceed the configured
 * MTU. Every frame is tagged with the VLAN identifier, priority code point,
 * destination MAC address, and egress interface. The frame index, total burst
 * size, and generation time are stored in the payload so that the receiver can
 * compute burst-level and frame-level statistics.
 */
void EthernetApplication::generateFrames() {
    int remLen = par("payload");
    int mtu = par("mtu");
    Packet *pkt = nullptr;
    int num = 0;
    int len;
    int pcp = par("priority");
    int vlanid = par("vlanid");

    int burst = ceil((double)remLen/(double)mtu);

    while(remLen > 0) {
        pkt = new Packet(par("name").stringValue());

        pkt->addTag<VlanReq>()->setVlanId(vlanid);
        pkt->addTag<PcpReq>()->setPcp(pcp);
        pkt->addTag<MacAddressReq>()->setDestAddress(remoteAddress);
        pkt->addTag<InterfaceReq>()->setInterfaceId(networkInterface->getInterfaceId());

        auto data = makeShared<EthernetDataPayload>();
        data->setFrame_num(num);
        data->setBurst_size(burst);
        data->setGenTime(simTime());
        num++;
        len = (remLen < mtu) ? remLen : mtu;
        data->setChunkLength(B(len));
        pkt->insertAtBack(data);

        queue.insert(pkt);
        remLen -= len;
    }

    transmitFrames();
    // NOTE: The burst transmission is assumed to complete before the next period.
}

/**
 * Consumes a frame delivered by the socket and emits delay and jitter
 * statistics.
 *
 * BurstDelay is emitted only for the last frame of a burst. FrameDelay and
 * AbsoluteJitter are emitted for every received frame.
 *
 * @param socket Socket that delivered the frame.
 * @param pkt Received frame; ownership is transferred to the callee.
 */
void EthernetApplication::socketDataArrived(TSNSocket *socket, Packet *pkt) {
    EV << "Arrivato pacchetto: " << pkt->getName() << endl;
    auto pay = pkt->peekAtFront<EthernetDataPayload>();
    int bs = pay->getBurst_size();
    int num = pay->getFrame_num();
    simtime_t e2ed;
    simsignal_t sig;
    simsignal_t absoluteJitter;
    simtime_t aJitter;


    if((bs-1) == num) {
        e2ed = simTime()-pay->getGenTime();
        sig = registerSignal("BurstDelay");
        emit(sig, e2ed);
    }

    e2ed = simTime()-pay->getGenTime();
    sig = registerSignal("FrameDelay");
    emit(sig, e2ed);

    absoluteJitter = registerSignal("AbsoluteJitter");
    aJitter = getAbsolutejitter(e2ed);
    emit(absoluteJitter,aJitter);

    delete pkt;
}

/**
 * Updates the observed end-to-end delay range and returns the difference
 * between the maximum and minimum delays.
 *
 * @param e2ed End-to-end delay of the current frame.
 * @return Absolute jitter observed so far.
 */
simtime_t EthernetApplication ::getAbsolutejitter(simtime_t e2ed) {
    if(e2ed < e2edMin)
        e2edMin = e2ed;

    if(e2ed > e2edMax)
        e2edMax = e2ed;

    return (e2edMax - e2edMin);

}

/**
 * Transmits queued frames, enforcing the configured inter-arrival time.
 *
 * If the inter-arrival time is zero, the next frame is transmitted
 * immediately through a recursive call. Otherwise, an InterTimer message is
 * scheduled to resume transmission later.
 */
void EthernetApplication::transmitFrames() {
    simtime_t it = par("interarrivalTime");
    if(!queue.isEmpty()) {
        auto pkt = queue.pop();
        socket.send(check_and_cast<Packet *>(pkt));

        if(!queue.isEmpty()) {
            if(it == 0) {
                transmitFrames();
            } else {
                cMessage *tim = new cMessage("InterTimer");
                scheduleAt(simTime()+it, tim);
            }
        }
    }
}

/**
 * Binds the socket to the configured flow name and schedules the first
 * frame-generation timer.
 *
 * @param operation Lifecycle operation that triggered the start.
 */
void EthernetApplication::handleStartOperation(LifecycleOperation *operation) {
    simtime_t stime = par("startTime");

    socket.bind(par("name").stringValue());

    if(stime >= 0) {
        cMessage *tim = new cMessage("StartTimer");
        scheduleAt(simTime()+stime, tim);
    }
}

/**
 * Handles a graceful stop operation.
 *
 * No module-specific cleanup is required.
 *
 * @param operation Lifecycle operation that triggered the stop.
 */
void EthernetApplication::handleStopOperation(LifecycleOperation *operation) {

}

/**
 * Handles a crash operation.
 *
 * No module-specific cleanup is required.
 *
 * @param operation Lifecycle operation that triggered the crash.
 */
void EthernetApplication::handleCrashOperation(LifecycleOperation *operation) {

}
