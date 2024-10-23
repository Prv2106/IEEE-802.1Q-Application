#include "TSNNetLayer.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ProtocolUtils.h"
#include "EthernetDataFrames_m.h"
#include "inet/common/socket/SocketBase.h"

Define_Module(TSNNetLayer);

void TSNNetLayer::initialize(int stage) {
    if(stage == INITSTAGE_NETWORK_LAYER) {
        registerProtocol(Protocol::tsn, gate("ifOut"), gate("ifIn"));
        // il metodo sopra è un'astrazione di quello che fa il modulo all'avvio (inizializzazione) ed è come se
        // il modulo dicesse ai message dispatcher: "guarda che se c'è una frame con TAG "protocollo TSN",
        // mandamelo al gate 'ifIn' (ultimo parametro), ed io lo mettero in coda al gate "ifOut" ".

        registerService(Protocol::tsn, gate("transportOut"), gate("transportIn"));
    }
}

void TSNNetLayer::handleMessage(cMessage *msg) {
    if(msg->arrivedOn("transportIn")) {
        // se arriva un pacchetto da sopra
        if(msg->isPacket()) {
            Packet *pkt = check_and_cast<Packet *>(msg);
            pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::tsn); // aggiungiamo il TAG tsn
            ensureEncapsulationProtocolReq(pkt, &Protocol::ieee8021qCTag);
            // incapsuliamo all'interno un altro TAG che serve a livello MAC (il Link layer sottostante)
            setDispatchProtocol(pkt);
            send(pkt, "ifOut");
            return;
        } else {
            // se il messaggio handleato non è un pacchetto, allora è un comando (di bind)
            Request *req = check_and_cast<Request *>(msg);
            if (auto bindCommand = dynamic_cast<TSNBindCommand *>(req->getControlInfo())) {
                // mettiamolo nella mappa
                int socketId = check_and_cast<Request *>(msg)->getTag<SocketReq>()->getSocketId();
                Socket *socket = new Socket(socketId);
                socket->flowName = bindCommand->getFlowName();
                socketIdToSocketMap[socketId] = socket;
            }
            delete msg;
            return;
        }
    } else {
        // quando arriva un messaggio da sotto (link layer)
        Packet *pkt = check_and_cast<Packet *>(msg);
        for (auto it : socketIdToSocketMap) {
            auto socket = it.second;
            if (socket->matches(pkt)) {
                auto packetCopy = pkt->dup();
                packetCopy->setKind(SOCKET_I_DATA);
                packetCopy->addTagIfAbsent<SocketInd>()->setSocketId(it.first);
                EV_INFO << "Sending " << packetCopy << " to socket " << it.first << ".\n";
                send(packetCopy, "transportOut");
            } // abbiamo, in sostanza, mandato il messaggio all'applicazione corrispondente!
        }
        delete msg;
        return;
    }
}

bool TSNNetLayer::Socket::matches(Packet *packet) {
    if (strcmp(packet->getName(), flowName.c_str()) == 0) {
        return true;
    }
    return false;
}
