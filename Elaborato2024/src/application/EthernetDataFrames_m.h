//
// Generated file, do not edit! Created by opp_msgtool 6.0 from application/EthernetDataFrames.msg.
//

#ifndef __INET_ETHERNETDATAFRAMES_M_H
#define __INET_ETHERNETDATAFRAMES_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif


namespace inet {

class EthernetDataPayload;
class TSNBindCommand;

}  // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/common/Protocol_m.h" // import inet.common.Protocol

#include "inet/common/socket/SocketCommand_m.h" // import inet.common.socket.SocketCommand

#include "inet/linklayer/common/MacAddress_m.h" // import inet.linklayer.common.MacAddress


namespace inet {

/**
 * Class generated from <tt>application/EthernetDataFrames.msg:9</tt> by opp_msgtool.
 * <pre>
 * class EthernetDataPayload extends inet::FieldsChunk
 * {
 *     int frame_num;
 *     int burst_size;
 *     simtime_t genTime;
 * }
 * </pre>
 */
class EthernetDataPayload : public ::inet::FieldsChunk
{
  protected:
    int frame_num = 0;
    int burst_size = 0;
    ::omnetpp::simtime_t genTime = SIMTIME_ZERO;

  private:
    void copy(const EthernetDataPayload& other);

  protected:
    bool operator==(const EthernetDataPayload&) = delete;

  public:
    EthernetDataPayload();
    EthernetDataPayload(const EthernetDataPayload& other);
    virtual ~EthernetDataPayload();
    EthernetDataPayload& operator=(const EthernetDataPayload& other);
    virtual EthernetDataPayload *dup() const override {return new EthernetDataPayload(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getFrame_num() const;
    virtual void setFrame_num(int frame_num);

    virtual int getBurst_size() const;
    virtual void setBurst_size(int burst_size);

    virtual ::omnetpp::simtime_t getGenTime() const;
    virtual void setGenTime(::omnetpp::simtime_t genTime);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const EthernetDataPayload& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, EthernetDataPayload& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>application/EthernetDataFrames.msg:15</tt> by opp_msgtool.
 * <pre>
 * class TSNBindCommand extends SocketBindCommand
 * {
 *     string flowName; // la bind viene fatta sul nome del flusso
 * }
 * </pre>
 */
class TSNBindCommand : public ::inet::SocketBindCommand
{
  protected:
    ::omnetpp::opp_string flowName;

  private:
    void copy(const TSNBindCommand& other);

  protected:
    bool operator==(const TSNBindCommand&) = delete;

  public:
    TSNBindCommand();
    TSNBindCommand(const TSNBindCommand& other);
    virtual ~TSNBindCommand();
    TSNBindCommand& operator=(const TSNBindCommand& other);
    virtual TSNBindCommand *dup() const override {return new TSNBindCommand(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual const char * getFlowName() const;
    virtual void setFlowName(const char * flowName);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const TSNBindCommand& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, TSNBindCommand& obj) {obj.parsimUnpack(b);}


}  // namespace inet


namespace omnetpp {

template<> inline inet::EthernetDataPayload *fromAnyPtr(any_ptr ptr) { return check_and_cast<inet::EthernetDataPayload*>(ptr.get<cObject>()); }
template<> inline inet::TSNBindCommand *fromAnyPtr(any_ptr ptr) { return check_and_cast<inet::TSNBindCommand*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __INET_ETHERNETDATAFRAMES_M_H

