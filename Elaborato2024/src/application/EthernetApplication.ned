package elaborato2024.application;

import inet.applications.contract.IApp;

// Periodic TSN traffic source and sink.
//
// Generates bursts of Ethernet frames at the configured period, transmits
// them through a TSN socket, and records end-to-end delay and jitter
// statistics for received frames.
simple EthernetApplication like IApp
{
    parameters:
        double startTime @unit(s) = default(-1s);        // Time at which frame generation starts; negative values disable it.
        volatile double period @unit(s) = default(0s);   // Interval between consecutive frame generations.
        double deadlineRel @unit(s) = default(0s);       // Relative deadline of the traffic stream.
        volatile int payload @unit(B) = default(0B);     // Payload size of each burst.
        int vlanid = default(0);                         // VLAN identifier applied to generated frames.
        int priority = default(0);                       // IEEE 802.1Q priority code point.
        double interarrivalTime @unit(s) = default(0s);  // Delay between consecutive frames of a burst.
        string name = default("");                       // Flow name used for socket binding and packet matching.
        string localAddress = default("");               // Local MAC address or resolvable module name.
        string remoteAddress = default("");              // Destination MAC address or resolvable module name.
        string netInterface = default("eth0");           // Egress network interface name.
        int mtu @unit(B) = default(1500B);               // Maximum payload size of a single frame.
        string interfaceTableModule = default("^.interfaceTable"); // Path to the interface table module.
        @display("i=block/app2");

        @signal[AbsoluteJitter];
        @signal[BurstDelay];
        @signal[FrameDelay];
        @statistic[AbsoluteJitter](record=vector,last);
        @statistic[BurstDelay](record=vector);
        @statistic[FrameDelay](record=vector);
    gates:
        input socketIn;   // Messages arriving from the TSN network layer.
        output socketOut; // Messages sent to the TSN network layer.
}