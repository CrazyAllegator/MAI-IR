//---------------------------------------------------------------------------
#ifndef TcpServerH
#define TcpServerH
//---------------------------------------------------------------------------
#include "Global.h"
#include "Exception.h"
#include "TcpClient.h"
//---------------------------------------------------------------------------
class TcpServer
{
  public:
    TcpServer();
    virtual ~TcpServer();

    String host()const;
    String ip()  const;
    unsigned short port() const;

    void setAddr(const String& addr);
    void setPort(unsigned short port);

    void bind();
    void bind(const String& addr,unsigned short port);
    void listen(int backlog=256);
    void accept();
    void close(bool nonTrow= false);

    virtual void workWithClient(TcpClient* client)=0;

private:
    SOCKET      socket_;
    HOSTENT*    hostent_;
    sockaddr_in addr_;
};
//---------------------------------------------------------------------------
#endif
