/**
 * @file TSNSocket.cc
 * @brief Implementation of the TSN socket endpoint.
 */

#include "TSNSocket.h"
#include "EthernetDataFrames_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/socket/SocketTag_m.h"

/** Inherited sendOut overload; delegates directly to SocketBase. */
void TSNSocket::sendOut(cMessage *msg) {
    SocketBase::sendOut(msg);
}

/**
 * Tags a bind request with the TSN dispatch protocol before forwarding it to
 * the base class.
 *
 * @param request Request to transmit.
 */
void TSNSocket::sendOut(Request *request) {
    request->addTag<DispatchProtocolReq>()->setProtocol(&Protocol::tsn);
    SocketBase::sendOut(request);
}

/**
 * Tags an outgoing packet with the TSN dispatch protocol before forwarding it
 * to the base class.
 *
 * @param packet Packet to transmit.
 */
void TSNSocket::sendOut(Packet *packet) {
    packet->addTag<DispatchProtocolReq>()->setProtocol(&Protocol::tsn);
    SocketBase::sendOut(packet);
}

/**
 * Binds the socket to a TSN flow name.
 *
 * A bind request carrying a TSNBindCommand is created, the socket is marked as
 * open, and the request is sent to the TSN network layer.
 *
 * @param flowName Flow name used for packet matching.
 */
void TSNSocket::bind(const char *flowName) {
    auto request = new Request("BIND", SOCKET_C_BIND);
    TSNBindCommand *ctrl = new TSNBindCommand();
    ctrl->setFlowName(flowName);
    request->setControlInfo(ctrl); // ControlInfo is a CObject pointer stored as a message attribute.
    isOpen_ = true;
    sendOut(request);
}

/**
 * Processes a message delivered by the network layer and dispatches it to the
 * registered callback.
 *
 * Data messages are forwarded as packets. A close indication marks the socket
 * as closed and notifies the callback. Messages of any other kind raise a
 * runtime error.
 *
 * @param msg Message to process.
 * @throws cRuntimeError when the message kind is not supported.
 */
void TSNSocket::processMessage(cMessage *msg) {
    ASSERT(belongsToSocket(msg));
    switch (msg->getKind()) {
        case SOCKET_I_DATA:
            if (callback)
                callback->socketDataArrived(this, check_and_cast<Packet *>(msg));
            else
                delete msg;
            break;
        case SOCKET_I_CLOSED:
            isOpen_ = false;
            if (callback)
                callback->socketClosed(this);
            delete msg;
            break;
        default:
            throw cRuntimeError("EthernetSocket: invalid msg kind %d, one of the ETHERNNET_I_xxx constants expected", msg->getKind());
            break;
    }
}
