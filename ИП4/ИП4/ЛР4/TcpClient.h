#ifndef TcpClientH
#define TcpClientH
//------------------------------------------------------------------------
#include "Global.h"
#include "Exception.h"
//------------------------------------------------------------------------
class TcpClient
{
public:
  unsigned long read(char* data, unsigned long size);
  unsigned long write(const char* data, unsigned long size);

  TcpClient();
  explicit TcpClient(SOCKET socket);
  ~TcpClient();

  String host()const;
  String ip()  const;
  unsigned short port() const;

  unsigned long timeOut()const;
  void setTimeOut(unsigned long timeOut);

  void setAddr(const String& addr);
  void setPort(int unsigned port);

  void connect();
  void connect(const String& addr,unsigned short port);
  void close(bool nonThrow= false);

  SOCKET      socket_;
  HOSTENT*    hostent_;
  sockaddr_in addr_;
  unsigned long timeOut_;
};
//---------------------------------------------------------------------------
#endif
