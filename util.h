#ifndef _util
#define _util

#include <stdio.h>


//template <class T> 
//ostream & operator << (ostream & os, const T &obj) 
//{
//  return obj::operator<<(os);
//};

int readall(const int fd, char *buf, const int len, const int oneshot=0, const int awaitblock=1);
int writeall(const int fd, const char *buf, const int len, const int oneshot=0, const int awaitblock=1);

void printhexnybble(FILE *out,const char lower);
void printhexbyte(FILE *out,const char h);
void printhexshort(FILE *out,const short h);
void printhexint(FILE *out,const int h);
void printhexbuffer(FILE *out, const char *buf, const int len);

void hexbytetobyte(const char hexbyte[2], char *byte);
void bytetohexbyte(const char byte, char hexbyte[2]);

void IPAddressToString(unsigned adx, char buf[16]);


bool CanReadNow(const int fd);
bool CanWriteNow(const int fd);
bool WaitForRead(const int fd);
bool WaitForWrite(const int fd); 

unsigned short OnesComplementSum(unsigned short *buf, int len);

struct SerializationException {
};


#include <iostream>
#include <algorithm>
#include <functional>


template <class T> 
inline ostream & operator<< (ostream &lhs, const T &rhs) { return rhs.Print(lhs);} ;

// gcc is broken
inline ostream & operator<< (ostream &lhs, char *rhs) { return lhs << ((const char*) rhs);}

template <class T>
struct PrintFunc : public unary_function<T, void>
{
  ostream& os;
  PrintFunc(ostream& out) : os(out) {}
  void operator() (T x) { os << x ; }
};

template <typename T>
T MAX(const T &lhs, const T &rhs)
{
  return (lhs>rhs) ? lhs : rhs;
};

template <typename T>
T MIN(const T &lhs, const T &rhs)
{
  return (lhs<rhs) ? lhs : rhs;
};

#endif
