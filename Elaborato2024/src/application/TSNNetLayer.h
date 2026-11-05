#ifndef __ELABORATO2024_TSNNETLAYER_H_
#define __ELABORATO2024_TSNNETLAYER_H_

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"

using namespace omnetpp;
using namespace inet;

/**
 * Minimal TSN network-layer dispatcher.
 *
 * Implements the INET INetworkLayer contract. Outgoing packets receive the
 * TSN protocol tag and an IEEE 802.1Q C-tag encapsulation request before being
 * forwarded to the link layer. Incoming packets are matched against the flow
 * names of the bound sockets and delivered to the corresponding application.
 */
class TSNNetLayer : public cSimpleModule {
  protected:
    /**
     * Socket registration entry keyed by socket identifier.
     *
     * An incoming packet matches the socket when the packet name equals the
     * registered flow name.
     */
    struct Socket {
        /** Socket identifier assigned by the application. */
        int socketId = -1;

        /** Flow name used for packet matching. */
        std::string flowName;

        /**
         * @param socketId Socket identifier.
         */
        Socket(int socketId) : socketId(socketId) {}

        /**
         * @param packet Incoming packet.
         * @return True when the packet name equals the registered flow name.
         */
        bool matches(Packet *packet);
    };

    /**
     * Registers the TSN protocol and service at the network-layer stage.
     *
     * @param stage Current initialization stage.
     */
    virtual void initialize(int stage) override;

    /** @return Total number of initialization stages. */
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /**
     * Handles bind requests from the transport layer and forwards packets
     * between the transport and link layers.
     *
     * @param msg Message to process.
     */
    virtual void handleMessage(cMessage *msg) override;

    /** Mapping from socket identifier to the registered socket. */
    std::map<int, Socket *> socketIdToSocketMap;
};

#endif
