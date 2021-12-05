#ifndef HTTPSERVER_H
#define HTTPSERVER_H
//---------------------------------------------------------------------
#include "TcpServer.h"
#include "Storage.h"
#include "Query.h"
#include "Encoding.h"
#include "Utils.h"
//---------------------------------------------------------------------
class HttpServer : public TcpServer
{
  public:
      HttpServer(Storage* storage);
      void start(unsigned short port);

  private:
      virtual void workWithClient(TcpClient* client) override;

      void writeOkResponseHeaders(TcpClient* client,size_t contentLength);
      void write404ResponseHeaders(TcpClient* client,size_t contentLength);

      void routeMain(TcpClient* client);
      void routeFile(TcpClient* client, const WString& filePath);

      bool parseRequestParams(const String& headers);

  private:
      Storage* storage_;
      const static size_t bufferSize_= 1024;

      WString queryString_;
      String  url_;
      size_t  page_= 1;
};
//---------------------------------------------------------------------
#endif // HTTPSERVER_H
