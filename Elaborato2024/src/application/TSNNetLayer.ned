package elaborato2024.application;

import inet.networklayer.contract.INetworkLayer;

// TSN network-layer dispatcher.
//
// Implements the INetworkLayer contract. The transportIn and transportOut
// gates connect to the transport layer, while ifIn and ifOut connect to the
// link layer. The module dispatches packets between the two layers.
simple TSNNetLayer like INetworkLayer {
    gates:
        input transportIn @labels(ITransportPacket/down);
        output transportOut @labels(ITransportPacket/up);
        input ifIn @labels(INetworkHeader/up);
        output ifOut @labels(INetworkHeader/down);
}
