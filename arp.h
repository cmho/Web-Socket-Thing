#ifndef _arp
#define _arp


#include "raw_ethernet_packet.h"
#include "ethernet.h"
#include "packet.h"
#include "ip.h"

typedef unsigned short ARPHWAddressSpaceType;
typedef unsigned short ARPProtoAddressSpaceType;
typedef unsigned char  ARPHWAddressLenType;
typedef unsigned char  ARPProtoAddressLenType;
typedef unsigned short ARPOpcodeType;

//
// Note that this supports only IP->Ethernet address translation
//

class ARPPacket : public Packet {
 public:
  enum OpcodeType {Request, Reply};

  ARPPacket();
  ARPPacket(const ARPPacket &rhs);
  ARPPacket(const RawEthernetPacket &rhs);
  ARPPacket(const Packet &rhs);
  ARPPacket(const OpcodeType     opcode, 
            const EthernetAddr   &sender_hw_addr,
            const IPAddress      &sender_ip_addr,
            const EthernetAddr   &target_hw_addr,
            const IPAddress      &target_ip_addr);
  const ARPPacket & operator=(const ARPPacket &rhs);
  virtual ~ARPPacket();

  bool IsIPToEthernet() const;
  bool IsIPToEthernetRequest() const;
  bool IsIPToEthernetReply() const;
  
  void GetHWAddressSpace(ARPHWAddressSpaceType &haddr) const;
  void SetHWAddressSpace(const ARPHWAddressSpaceType &haddr);

  void GetProtocolAddressSpace(ARPProtoAddressSpaceType &paddr) const;
  void SetProtocolAddressSpace(const ARPProtoAddressSpaceType &paddr);

  void GetHWAddressLen(ARPHWAddressLenType &hlen) const;
  void SetHWAddressLen(const ARPHWAddressLenType &hlen);

  void GetProtoAddressLen(ARPProtoAddressLenType &hlen) const;
  void SetProtoAddressLen(const ARPProtoAddressLenType &hlen);

  void GetOpcode(OpcodeType &opcode) const;
  void SetOpcode(const OpcodeType &opcode);

  void SetSenderEthernetAddr(const EthernetAddr &sender_hw_addr);
  void GetSenderEthernetAddr(EthernetAddr &sender_hw_addr) const;

  void SetSenderIPAddr(const IPAddress &sender_ip_addr);
  void GetSenderIPAddr(IPAddress &sender_ip_addr) const;

  void SetTargetEthernetAddr(const EthernetAddr &target_hw_addr);
  void GetTargetEthernetAddr(EthernetAddr &target_hw_addr) const;

  void SetTargetIPAddr(const IPAddress &target_ip_addr);
  void GetTargetIPAddr(IPAddress &target_ip_addr) const;

  ostream & Print(ostream &os) const;

};

// Note: local

struct ARPRequestResponse {
  IPAddress    ipaddr;
  EthernetAddr ethernetaddr;
  enum Flag {REQUEST=1, RESPONSE_OK=2, RESPONSE_UNKNOWN=4} flag;

  ARPRequestResponse();
  ARPRequestResponse(const ARPRequestResponse &rhs);
  ARPRequestResponse(const IPAddress &ip, 
		     const EthernetAddr &hw,
		     const Flag          flag);
  ARPRequestResponse & operator=(const ARPRequestResponse &rhs);
  virtual ~ARPRequestResponse();

  void Serialize(const int fd) const;
  void Unserialize(const int fd);

  ostream & Print(ostream &os) const;
};

#include <hash_map>





struct eqipaddress {
  bool operator()(const IPAddress lhs, const IPAddress rhs) const { return lhs==rhs;}
};

struct hashipaddress : public hash<unsigned> {
  hash<unsigned> H;
  size_t operator()(const IPAddress &x) const { return H((unsigned)x); }
};


ostream & operator<< (ostream &os, const pair<IPAddress,ARPRequestResponse> &pair);


class ARPCache {
 private:
  typedef hash_map<IPAddress,ARPRequestResponse,hashipaddress,eqipaddress> DataType;
  DataType data;
 
 public:
  void Update(const ARPRequestResponse &x);
  void Delete(const IPAddress &a);
  void Lookup(ARPRequestResponse &x) const;
  
  ostream & Print(ostream &os) const;
};


const short ARP_REQUEST_OPCODE=0x0001;
const short ARP_REPLY_OPCODE=0x0002;

const short ETHERNET_HW_ADXSPACE=0x0001;
const short IP_PROTO_ADXSPACE=0x0800;


#endif
