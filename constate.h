#ifndef _constate
#define _constate

#include "sockint.h"
#include <deque>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <typeinfo>

// connection state mapping

// To be interchangeably used with timevals
// Note this is NOT virtual
struct Time : public timeval
{
  Time(const Time &rhs);
  Time(const timeval &rhs);
  Time(const double time);
  Time(const unsigned sec, const unsigned usec);
  Time(); // gets the current time

  Time & operator=(const Time &rhs);
  Time & operator=(const double &rhs);

  void SetToCurrentTime();

  operator double() const;

  bool operator<(const Time &rhs) const;
  bool operator>(const Time &rhs) const;
  bool operator==(const Time &rhs) const;

  ostream & Print(ostream &os) const;
};


template <class STATE>
struct ConnectionToStateMapping {
  Connection connection;
  Time       timeout;
  STATE      state;
  bool       bTmrActive;
  
  ConnectionToStateMapping(const ConnectionToStateMapping<STATE> &rhs) : 
    connection(rhs.connection), timeout(rhs.timeout), state(rhs.state), bTmrActive(rhs.bTmrActive) {}
  ConnectionToStateMapping(const Connection &c, 
			   const Time &t,
			   const STATE &s,
			   const bool &b) :
    connection(c), timeout(t), state(s), bTmrActive(b) {}
  ConnectionToStateMapping() : connection(), timeout(), state(), bTmrActive() {}

  ConnectionToStateMapping<STATE> & operator=(const ConnectionToStateMapping<STATE> &rhs) {
    connection=rhs.connection; timeout=rhs.timeout; state=rhs.state;
    bTmrActive=rhs.bTmrActive; return *this;
  }
  bool MatchesSource(const Connection &rhs) const {
    return connection.MatchesSource(rhs);
  }
  bool MatchesDest(const Connection &rhs) const {
    return connection.MatchesDest(rhs);
  }
  bool MatchesProtocol(const Connection &rhs) const {
    return connection.MatchesProtocol(rhs);
  }
  bool Matches(const Connection &rhs) const {
    return connection.Matches(rhs);
  }
  ostream & Print(ostream &os) const {
    os << "ConnectionToStateMapping(connection="<<connection
       <<", timeout="<<timeout<<", state="<<state<<", bTmrActive="<<bTmrActive<<")";
    return os;
  }
};

template <class STATE>
class ConnectionList : public deque<ConnectionToStateMapping<STATE> >
{
 public:
  ConnectionList(const ConnectionList &rhs) : deque<ConnectionToStateMapping<STATE> >(rhs) {}
  ConnectionList() {}

  ConnectionList<STATE>::iterator FindEarliest() {
    ConnectionList<STATE>::iterator ptr=end();
    ConnectionList<STATE>::iterator i=begin();

    // No connections in list
    if(empty())
	return end();

    // 1 connection in list
    if(size() == 1) {
	if((*i).bTmrActive == true)
	    return begin();
	else {
	    return end();
	}
    }

    // More than one connection in list
    Time min=(*i).timeout;
    for (; i!=end(); ++i) {
      if ((*i).bTmrActive == true && (*i).timeout <= min) {
	min=(*i).timeout;
	ptr=i;
      }
    }
    return ptr;
  }

  ConnectionList<STATE>::iterator FindMatching(const Connection &rhs) {
    for (ConnectionList<STATE>::iterator i=begin(); i!=end(); ++i) {
      if ((*i).Matches(rhs)) {
	return i;
      }
    }
    return end();
  }
  ConnectionList<STATE>::iterator FindMatchingSource(const Connection &rhs) {
    for (ConnectionList<STATE>::iterator i=begin(); i!=end(); ++i) {
      if ((*i).MatchesSource(rhs)) {
	return i;
      }
    }
    return end();
  }
   ConnectionList<STATE>::iterator FindMatchingDest(const Connection &rhs) {
    for (ConnectionList<STATE>::iterator i=begin(); i!=end(); ++i) {
      if ((*i).MatchesDest(rhs)) {
	return i;
      }
    }
    return end();
  }
   ConnectionList<STATE>::iterator FindMatchingProtocol(const Connection &rhs) {
    for (ConnectionList<STATE>::iterator i=begin(); i!=end(); ++i) {
      if ((*i).MatchesProtocol(rhs)) {
	return i;
      }
    }
    return end();
  }

   ostream & Print(ostream &os) const {
     os << "ConnectionList(";
     for (const_iterator i=begin(); i!=end(); ++i) {
       os << (*i);
     }
     os << ")";
     return os;
   }
};



#endif
