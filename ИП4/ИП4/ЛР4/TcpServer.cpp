// ---------------------------------------------------------------------------
#include "TcpServer.h"
// ---------------------------------------------------------------------------
TcpServer::TcpServer() :
  socket_(INVALID_SOCKET),
  hostent_(0)
{
  addr_.sin_family= AF_INET;
  socket_= ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_== INVALID_SOCKET)
    throw  WsaException();
}
// ---------------------------------------------------------------------------
/* virtual */ TcpServer::~TcpServer()
{

}
// ---------------------------------------------------------------------------
void TcpServer::bind()
{
  if(::bind(socket_,(sockaddr*)&addr_, sizeof(addr_))== SOCKET_ERROR)
    throw  WsaException();
}
// ---------------------------------------------------------------------------
void TcpServer::bind(const String& addr, unsigned short port)
{
  setPort(port);
  setAddr(addr);
  bind();
}
// ---------------------------------------------------------------------------
void TcpServer::listen(int backlog)
{
  if(::listen(socket_, backlog)== SOCKET_ERROR)
    throw  WsaException();
}
// ---------------------------------------------------------------------------
void TcpServer::accept()
{
  SOCKET clientSocket= INVALID_SOCKET;
  sockaddr_in client_addr;
  int client_addr_size= sizeof(client_addr);
  do
  {
    clientSocket= ::accept(socket_, (sockaddr*)&client_addr, &client_addr_size);    
    TcpClient client(clientSocket);
    workWithClient(&client);
  }
  while (clientSocket!= INVALID_SOCKET);

  throw  WsaException();
}
// ---------------------------------------------------------------------------
void TcpServer::close(bool nonThrow)
{
  if(shutdown(socket_, SD_BOTH)== SOCKET_ERROR&& !nonThrow)
  {
    throw  WsaException();
  }
  if(closesocket(socket_)== SOCKET_ERROR && !nonThrow)
  {
    throw  WsaException();
  }
}
// ---------------------------------------------------------------------------
String TcpServer::host() const
{
  return hostent_? hostent_->h_name: "";
}
// ---------------------------------------------------------------------------
String TcpServer::ip() const
{
  return inet_ntoa(addr_.sin_addr);
}
// ---------------------------------------------------------------------------
unsigned short TcpServer::port() const
{
  return ntohs(addr_.sin_port);
}
// ---------------------------------------------------------------------------
void TcpServer::setPort(unsigned short port)
{
  addr_.sin_port= htons(port);
}
// ---------------------------------------------------------------------------
void TcpServer::setAddr(const String& addr)
{
  if (inet_addr(addr.c_str())!= INADDR_NONE)
    addr_.sin_addr.s_addr= inet_addr(addr.c_str());
  else
  {
    hostent_= gethostbyname(addr.c_str());
    if (hostent_)
    {
      memcpy( &addr_.sin_addr,
              hostent_->h_addr_list[0],
              hostent_->h_length);
    }
    else
    {
      throw  WsaException();
    }
  }
}
// ---------------------------------------------------------------------------
