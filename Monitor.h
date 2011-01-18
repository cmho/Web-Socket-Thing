#ifndef _Monitor
#define _Monitor

#include <iostream>
#include <string>
#include "Minet.h"


enum MinetOpType {
  MINET_INIT,
  MINET_DEINIT,
  MINET_SEND,
  MINET_RECEIVE,
  MINET_GETNEXTEVENT,
  MINET_CONNECT,
  MINET_ACCEPT,
  MINET_CLOSE,
  MINET_SENDTOMONITOR,
  MINET_NOP,
};

ostream & operator<<(ostream &os, const MinetOpType &op);



struct MinetMonitoringEventDescription {
  double         timestamp;
  MinetModule    source;
  MinetModule    from;
  MinetModule    to;
  MinetDatatype  datatype;
  MinetOpType    optype;

  MinetMonitoringEventDescription();
  MinetMonitoringEventDescription(const MinetMonitoringEventDescription &rhs);
  virtual ~MinetMonitoringEventDescription();
  
  virtual const MinetMonitoringEventDescription & operator= (const MinetMonitoringEventDescription &rhs);

  virtual void Serialize(const int fd) const;
  virtual void Unserialize(const int fd);

  virtual ostream & Print(ostream &os) const;
};
  

struct MinetMonitoringEvent : public string {
  MinetMonitoringEvent();
  MinetMonitoringEvent(const char *s);
  MinetMonitoringEvent(const string &s);
  MinetMonitoringEvent(const MinetMonitoringEvent &rhs);
  virtual ~MinetMonitoringEvent();

  virtual const MinetMonitoringEvent & operator= (const MinetMonitoringEvent &rhs);

  virtual void Serialize(const int fd) const;
  virtual void Unserialize(const int fd);

  virtual ostream & Print(ostream &os) const;
};


#endif



  
