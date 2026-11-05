/**
 * @file TSNNetLayer.cc
 * @brief Implementation of the TSN network-layer dispatcher.
 */

#include "TSNNetLayer.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ProtocolUtils.h"
#include "EthernetDataFrames_m.h"
#include "inet/common/socket/SocketBase.h"

Define_Module(TSNNetLayer);

/**
 * Registers the TSN protocol and service at the network-layer stage.
 *
 * registerProtocol() declares the module as the network-layer handler for the
 * TSN protocol: frames tagged with Protocol::tsn received from the link layer
 * are delivered through the ifIn gate, while frames sent through ifOut are
 * dispatched towards the link layer. registerService() exposes the TSN service
 * to the transport layer through the transportIn and transportOut gates.
 *
 * @param stage Current initialization stage.
 */
void TSNNetLayer::initialize(int stage) {
    if(stage == INITSTAGE_NETWORK_LAYER) {
        registerProtocol(Protocol::tsn, gate("ifOut"), gate("ifIn"));

        registerService(Protocol::tsn, gate("transportOut"), gate("transportIn"));
    }
}

/**
 * Handles bind requests from the transport layer and forwards packets
 * between the transport and link layers.
 *
 * @param msg Message to process.
 */
void TSNNetLayer::handleMessage(cMessage *msg) {
    if(msg->arrivedOn("transportIn")) {
        // Packet arriving from the transport layer.
        if(msg->isPacket()) {
            Packet *pkt = check_and_cast<Packet *>(msg);
            pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::tsn);
            // Request the IEEE 802.1Q C-tag encapsulation handled by the link layer.
            ensureEncapsulationProtocolReq(pkt, &Protocol::ieee8021qCTag);
            setDispatchProtocol(pkt);
            send(pkt, "ifOut");
            return;
        } else {
            // Non-packet messages are bind commands.
            Request *req = check_and_cast<Request *>(msg);
            if (auto bindCommand = dynamic_cast<TSNBindCommand *>(req->getControlInfo())) {
                // Register the socket in the dispatch map.
                int socketId = check_and_cast<Request *>(msg)->getTag<SocketReq>()->getSocketId();
                Socket *socket = new Socket(socketId);
                socket->flowName = bindCommand->getFlowName();
                socketIdToSocketMap[socketId] = socket;
            }
            delete msg;
            return;
        }
    } else {
        // Message arriving from the link layer.
        Packet *pkt = check_and_cast<Packet *>(msg);
        for (auto it : socketIdToSocketMap) {
            auto socket = it.second;
            if (socket->matches(pkt)) {
                auto packetCopy = pkt->dup();
                packetCopy->setKind(SOCKET_I_DATA);
                packetCopy->addTagIfAbsent<SocketInd>()->setSocketId(it.first);
                EV_INFO << "Sending " << packetCopy << " to socket " << it.first << ".\n";
                send(packetCopy, "transportOut");
            }
        }
        delete msg;
        return;
    }
}

/**
 * Checks whether an incoming packet belongs to the registered flow.
 *
 * @param packet Incoming packet.
 * @return True when the packet name equals the registered flow name.
 */
bool TSNNetLayer::Socket::matches(Packet *packet) {
    if (strcmp(packet->getName(), flowName.c_str()) == 0) {
        return true;
    }
    return false;
}
