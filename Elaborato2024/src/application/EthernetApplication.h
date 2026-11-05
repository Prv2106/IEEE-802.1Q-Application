#ifndef __ELABORATO2024_ETHERNETAPPLICATION_H_
#define __ELABORATO2024_ETHERNETAPPLICATION_H_

#include <omnetpp.h>
#include "inet/applications/base/ApplicationBase.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "TSNSocket.h"

using namespace omnetpp;
using namespace inet;


/**
 * Periodic source and sink for TSN traffic streams.
 *
 * The module generates bursts of Ethernet frames at the configured period,
 * transmits them through a TSNSocket on the custom TSN network layer, and
 * consumes the frames delivered back by the socket. For every received frame
 * it emits end-to-end delay and absolute-jitter statistics.
 */
class EthernetApplication : public ApplicationBase, public TSNSocket::ICallback {
  protected:
    /**
     * Initializes the socket callback and output gate, resolves the local and
     * remote MAC addresses, and locates the egress network interface.
     *
     * @param stage Current initialization stage.
     */
    virtual void initialize(int stage) override;

    /** @return Total number of initialization stages. */
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    /**
     * Dispatches self-messages, frame-generation timers, inter-arrival
     * timers, and messages received from the socket.
     *
     * @param msg Message to process.
     */
    virtual void handleMessageWhenUp(cMessage *msg) override;

    /**
     * Binds the socket to the configured flow name and schedules the first
     * frame-generation timer.
     *
     * @param operation Lifecycle operation that triggered the start.
     */
    virtual void handleStartOperation(LifecycleOperation *operation) override;

    /** @param operation Lifecycle operation that triggered the stop. */
    virtual void handleStopOperation(LifecycleOperation *operation) override;

    /** @param operation Lifecycle operation that triggered the crash. */
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    /**
     * Consumes a frame delivered by the socket and emits the burst-delay,
     * frame-delay, and absolute-jitter statistics.
     *
     * @param socket Socket that delivered the frame.
     * @param packet Received frame; ownership is transferred to the callee.
     */
    virtual void socketDataArrived(TSNSocket *socket, Packet *packet) override;

    /**
     * Handles an error indication delivered by the socket.
     *
     * @param socket Socket that delivered the indication.
     * @param indication Error indication.
     */
    virtual void socketErrorArrived(TSNSocket *socket, Indication *indication) { };

    /**
     * Handles the closure of the socket.
     *
     * @param socket Socket that was closed.
     */
    virtual void socketClosed(TSNSocket *socket) { }

    /**
     * Updates the observed end-to-end delay range and returns the difference
     * between the maximum and minimum delays.
     *
     * @param e2ed End-to-end delay of the current frame.
     * @return Absolute jitter observed so far.
     */
    virtual simtime_t getAbsolutejitter(simtime_t e2ed);

    /** Generates one burst of frames and enqueues them for transmission. */
    virtual void generateFrames();

    /** Transmits queued frames, enforcing the configured inter-arrival time. */
    virtual void transmitFrames();

    /** Frames waiting for transmission. */
    cPacketQueue queue;

    /** Local MAC address used for multicast registration. */
    MacAddress localAddress;

    /** Destination MAC address of generated frames. */
    MacAddress remoteAddress;

    /** Egress network interface. */
    NetworkInterface *networkInterface = nullptr;

    /** Socket connected to the TSN network layer. */
    TSNSocket socket;

    /** Maximum end-to-end delay observed so far. */
    simtime_t e2edMax = 0;

    /** Minimum end-to-end delay observed so far. */
    simtime_t e2edMin = 1;
};

#endif
