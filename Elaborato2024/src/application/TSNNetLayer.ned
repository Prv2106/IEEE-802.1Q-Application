package elaborato2024.application;

import inet.networklayer.contract.INetworkLayer;

simple TSNNetLayer like INetworkLayer {
    gates:
        input transportIn @labels(ITransportPacket/down);
        output transportOut @labels(ITransportPacket/up);
        input ifIn @labels(INetworkHeader/up);
        output ifOut @labels(INetworkHeader/down);
}

// implementa l'interfaccia di netwrok layer...
// i primi due gates sono quelli che permettono il collgemento verso il transport layer
// gli ultimi due sono quelli che permettono il collegamento verso il link layer

// Di fatto, è un dispatcher!
