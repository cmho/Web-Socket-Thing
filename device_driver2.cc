#include <unistd.h>
#include <stdio.h>
extern "C" {
#include <libnet.h>
#include <pcap.h>
}
#include <sys/poll.h>
#include "netinet/if_ether.h"

#include "Minet.h"

//
// Define this to be nonzero if you're using libnet 1.1 instead of 1.0
//
#define LIBNET11 1


/*
   Simpler device driver for Minet
   No signals are used here.
*/


// device config
//
char *device;

// libnet stuff
char net_errbuf[LIBNET_ERRORBUF_SIZE];
#if LIBNET11
libnet_t *net_interface;
#else
struct libnet_link_int *net_interface;
#endif

// libpcap stuff
const int pcap_proglen=10240;
char pcap_program[pcap_proglen];
pcap_t *pcap_interface;
char pcap_errbuf[PCAP_ERRBUF_SIZE];
struct bpf_program pcap_filter;
bpf_u_int32 pcap_net, pcap_mask;
int pcap_fd;

// minet stuff
// note that this currently will only work with the fifo-based 
// communication.
MinetHandle ethermux_handle;
MinetHandle pcap_handle;

void Init()
{
  // get configuration
  if (!(device=getenv("MINET_ETHERNETDEVICE"))) {
    cerr << "Please set MINET_ETHERNETDEVICE\n";
    exit(-1);
  }
  
  // establish libnet session
#if LIBNET11
  net_interface=libnet_init(LIBNET_LINK_ADV,device,net_errbuf);
#else
  net_interface=libnet_open_link_interface(device,net_errbuf);
#endif
  if (net_interface==NULL) { 
    cerr<<"Can't open interface: "<<net_errbuf<<endl;
    exit(-1);
  }

  // establish pcap filter
  if (pcap_lookupnet(device,&pcap_net,&pcap_mask,pcap_errbuf)) {
    cerr<<"Can't get net and mask for "<<device<<": "<<pcap_errbuf<<endl;
    exit(-1);
  }
  cerr << "pcap_net="<<pcap_net<<", pcap_mask="<<pcap_mask<<endl;
  if ((pcap_interface=pcap_open_live(device,1518,1,0,pcap_errbuf))==NULL) { 
    cerr<< "Can't open "<<device<<":"<<pcap_errbuf<<endl;
    exit(-1);
  }
  char *ip=getenv("MINET_IPADDR");
  if (ip==0) { 
    cerr << "Set MINET_IPADDR"<<endl;
    exit(-1);
  }
  //
  // This is crucial.  It controls what your students will see
  //
  sprintf(pcap_program,"host %s or arp",ip);
  cerr <<"pcap_program='"<<pcap_program<<"'"<<endl;
  if (pcap_compile(pcap_interface,&pcap_filter,pcap_program,0,pcap_mask)) {
    cerr<<"Can't compile filter\n";
    exit(-1);
  }
  if (pcap_setfilter(pcap_interface,&pcap_filter)) { 
    cerr<<"Can't set filter\n";
    exit(-1);
  }
  pcap_fd=pcap_fileno(pcap_interface);

  // connect to the ethernet mux
  MinetInit(MINET_DEVICE_DRIVER);
  ethermux_handle=MinetIsModuleInConfig(MINET_ETHERNET_MUX) ? MinetAccept(MINET_ETHERNET_MUX) : MINET_NOHANDLE;
  // register pcap as an external connection
  pcap_handle=MinetAddExternalConnection(pcap_fd,pcap_fd);
}  

void ProcessIncoming()
{
  struct pcap_pkthdr header;
  const u_char *packet;

  packet=pcap_next(pcap_interface,&header);

  if (packet==NULL) {
    cerr <<"pcap_next returned a null pointer\n";
    exit(-1);
  }

  RawEthernetPacket p((const char *)packet,(unsigned)(header.len));

  MinetSend(ethermux_handle,p);
}

void ProcessOutgoing()
{
  RawEthernetPacket p;

  MinetReceive(ethermux_handle,p);

  //
  //
  // Make sure that the packet is allowed to be sent
  //
  Packet cp(p);
  cp.ExtractHeaderFromPayload<EthernetHeader>(ETHERNET_HEADER_LEN);
  EthernetHeader eh=cp.FindHeader(Headers::EthernetHeader);
  EthernetAddr ea;
  EthernetProtocol ep;
  eh.GetSrcAddr(ea);
  eh.GetProtocolType(ep);
  
  
  if (ea!=MyEthernetAddr || (ep!=PROTO_ARP && ep!=PROTO_IP)) {
    cerr << "discarding packet "<<p<<endl;
    MinetSendToMonitor(MinetMonitoringEvent("dropped illegal outgoing packet"));
    return;
  }
  

#if LIBNET11
  if (libnet_adv_write_link(net_interface,
			      (u_char *)(p.data),
			      p.size)<0) {
#else
  if (libnet_write_link_layer(net_interface,
			      (const char *)device,
			      (u_char *)(p.data),
			      p.size)<0) {
#endif
      cerr << "Can't write output packet to link\n";
      exit(-1);
  }
}


void Run()
{
  MinetSendToMonitor(MinetMonitoringEvent("device_driver2 operating"));
  cerr << "device_driver2 operating" << endl;

  MinetEvent event;

  while (MinetGetNextEvent(event)==0) {
    if (event.eventtype!=MinetEvent::Dataflow 
	|| event.direction!=MinetEvent::IN) {
      MinetSendToMonitor(MinetMonitoringEvent("Unknown event ignored."));
      cerr << "Unknown event ignored." << endl;
    } else {
      if (event.handle==ethermux_handle) {
	ProcessOutgoing();
      } else if (event.handle==pcap_handle) { 
	ProcessIncoming();
      } else {
	MinetSendToMonitor(MinetMonitoringEvent("Unknown handle ignored."));
	cerr << "Unknown handle ignored." << endl;
      }
    }
  }
}
    




int main(int argc, char *argv[])
{
  Init();
  Run();
}


	

  
  
  
