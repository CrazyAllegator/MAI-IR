//---------------------------------------------------------------------------
#include "TcpClient.h"
//---------------------------------------------------------------------------
unsigned long TcpClient::read(char *data, unsigned long size)
{
  int length= ::recv(socket_, data, size, NULL);
  if(length == SOCKET_ERROR)
  {
    unsigned long error= WSAGetLastError();
    switch(error)
    {
       case WSAETIMEDOUT: throw WsaTimeOutException(error);
       default:           throw WsaException(error);
    }
  }
  return length;
}
// ---------------------------------------------------------------------------
unsigned long TcpClient::write(const char* data, unsigned long size)
{
  int length = ::send(socket_, data, size, NULL);
  if(length == SOCKET_ERROR)
  {
    unsigned long error= WSAGetLastError();
    switch(error)
    {
       case WSAETIMEDOUT: throw WsaTimeOutException(error);
       default:           throw WsaException(error);
    }
  }
  return length;
}
//----------------------- TcpClient -----------------------------------------
TcpClient::TcpClient()
  : socket_(INVALID_SOCKET),
    hostent_(0),
    timeOut_(30000)
{
  addr_.sin_family= AF_INET;
  socket_= ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(socket_==INVALID_SOCKET)
    throw WsaException();
}
//---------------------------------------------------------------------------
TcpClient::TcpClient(SOCKET socket)
  :socket_(socket),
    hostent_(0),
    timeOut_(0)
{
  if(socket_==INVALID_SOCKET)
    throw WsaException();
}
//---------------------------------------------------------------------------
TcpClient::~TcpClient()
{
  close(true);
}
//---------------------------------------------------------------------------
void TcpClient::connect(const String& addr,
                        unsigned short port)
{
  setPort(port);
  setAddr(addr);
  TcpClient::connect();
}
//---------------------------------------------------------------------------
void TcpClient::connect()
{
  if(::connect(socket_,
               (sockaddr*)&addr_,
               sizeof(addr_)) == SOCKET_ERROR)
  {
    unsigned long error= WSAGetLastError();
    switch(error)
    {
       case WSAETIMEDOUT: throw WsaTimeOutException(error);
       default:           throw WsaException(error);
    }
  }

  if(timeOut_==INFINITE)
    return;

  if(setsockopt( socket_,
                 SOL_SOCKET,
                 SO_RCVTIMEO,
                 (char *)&timeOut_,
                 sizeof(timeOut_) ) == SOCKET_ERROR)
    throw WsaException();

  if(setsockopt( socket_,
                 SOL_SOCKET,
                 SO_SNDTIMEO,
                 (char *)&timeOut_,
                 sizeof(timeOut_) ) == SOCKET_ERROR)
    throw WsaException();
}
//---------------------------------------------------------------------------
void TcpClient::close(bool nonThrow)
{
  if(shutdown(socket_,SD_BOTH)==SOCKET_ERROR && !nonThrow)
  {
    throw WsaException();
  }
  if(closesocket(socket_)==SOCKET_ERROR && !nonThrow)
  {
    throw WsaException();
  }
}
//---------------------------------------------------------------------------
String TcpClient::host()const
{
  return hostent_?hostent_->h_name:"";
}
//---------------------------------------------------------------------------
String TcpClient::ip()  const
{
  return inet_ntoa(addr_.sin_addr);
}
//---------------------------------------------------------------------------
unsigned short TcpClient::port() const
{
  return ntohs(addr_.sin_port);
}
//---------------------------------------------------------------------------
unsigned long TcpClient::timeOut() const
{
  return timeOut_;
}
//---------------------------------------------------------------------------
void TcpClient::setTimeOut(unsigned long timeOut)
{
  if(timeOut_ != timeOut)
  {
    timeOut_= timeOut;

    if(socket_==INVALID_SOCKET)
      return;

    if(setsockopt( socket_,
                   SOL_SOCKET,
                   SO_RCVTIMEO,
                   (char *)&timeOut_,
                   sizeof(timeOut_) ) == SOCKET_ERROR)
      throw WsaException();

    if(setsockopt(socket_,
                  SOL_SOCKET,
                  SO_SNDTIMEO,
                  (char *)&timeOut_,
                  sizeof(timeOut_) ) == SOCKET_ERROR)
      throw WsaException();
  }
}
//---------------------------------------------------------------------------
void TcpClient::setPort(unsigned int port)
{
  addr_.sin_port= htons(port);
}
//---------------------------------------------------------------------------
void TcpClient::setAddr(const String& addr)
{
  if (inet_addr(addr.c_str())!=INADDR_NONE)
    addr_.sin_addr.s_addr= inet_addr(addr.c_str());
  else
  {
    hostent_= gethostbyname(addr.c_str());
    if(hostent_)
    {
      memcpy(&addr_.sin_addr, hostent_->h_addr_list[0], hostent_->h_length);
    }
    else
    {
      throw WsaException();
    }
  }
}
//---------------------------------------------------------------------------