import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.common.Protocol;
import inet.common.socket.SocketCommand;
import inet.linklayer.common.MacAddress;

namespace inet;

// Application payload carried by every generated Ethernet frame.
class EthernetDataPayload extends inet::FieldsChunk {
    int frame_num;      // Zero-based index of the frame within the burst.
    int burst_size;     // Total number of frames in the burst.
    simtime_t genTime;  // Simulation time at which the frame was generated.
}

// Socket bind command that identifies a TSN flow by name.
class TSNBindCommand extends SocketBindCommand {
    string flowName; // Name of the flow to which the socket is bound.
}