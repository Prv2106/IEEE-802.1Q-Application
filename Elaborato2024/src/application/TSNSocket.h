#ifndef APPLICATION_TSNSOCKET_H_
#define APPLICATION_TSNSOCKET_H_

#include "inet/common/Protocol.h"
#include "inet/common/packet/Message.h"
#include "inet/common/socket/SocketBase.h"
#include "inet/networklayer/common/NetworkInterface.h"

using namespace inet;

/**
 * Socket endpoint for TSN traffic.
 *
 * Extends INET SocketBase with flow-based binding. Outgoing requests and
 * packets are tagged with the TSN dispatch protocol, while incoming messages
 * are dispatched to the registered callback.
 */
class TSNSocket : public SocketBase {
public:
    /**
     * Callback interface for socket events.
     */
    class INET_API ICallback {
      public:
        virtual ~ICallback() {}

        /**
         * Notifies about the arrival of a data packet.
         *
         * @param socket Socket that received the packet.
         * @param packet Received packet; ownership is transferred to the callee.
         */
        virtual void socketDataArrived(TSNSocket *socket, Packet *packet) = 0;

        /**
         * Notifies about the arrival of an error indication.
         *
         * @param socket Socket that received the indication.
         * @param indication Error indication; ownership is transferred to the callee.
         */
        virtual void socketErrorArrived(TSNSocket *socket, Indication *indication) = 0;

        /**
         * Notifies about the closure of the socket.
         *
         * @param socket Socket that was closed.
         */
        virtual void socketClosed(TSNSocket *socket) = 0;
    };

  protected:
    /** Registered callback object; not owned by the socket. */
    ICallback *callback = nullptr;

  protected:
    /** Inherited sendOut overload; delegates directly to SocketBase. */
    virtual void sendOut(cMessage *msg) override;

    /** Tags bind requests with the TSN dispatch protocol. */
    virtual void sendOut(Request *request) override;

    /** Tags outgoing packets with the TSN dispatch protocol. */
    virtual void sendOut(Packet *packet) override;

  public:
    /** @name Opening and closing connections, sending data */
    //@{
    /**
     * Sets the callback object used to notify socket events.
     *
     * The callback object may be the application module itself when it also
     * implements ICallback, or a dedicated object. The socket does not take
     * ownership of the callback and never deletes it.
     *
     * @param callback Callback object.
     */
    void setCallback(ICallback *callback) { this->callback = callback; }

    /**
     * Binds the socket to a TSN flow name.
     *
     * @param floName Flow name used for packet matching.
     */
    void bind(const char *floName);
    //@}

    /** @name Handling of messages arriving from the TSN network layer */
    //@{
    /**
     * Processes a message delivered by the network layer and forwards it to
     * the callback.
     *
     * @param msg Message to process.
     * @throws cRuntimeError when the message kind is not supported.
     */
    virtual void processMessage(cMessage *msg) override;
    //@}
};

#endif /* APPLICATION_TSNSOCKET_H_ */
