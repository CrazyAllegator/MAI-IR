#include "HttpServer.h"

#include <stdio.h>

//---------------------------------------------------------------------
template<typename CharT>
void stringReplace(user::basic_string<CharT>& src,
                   const user::basic_string<CharT>& target,
                   const user::basic_string<CharT>& replacement)
{
  typename user::basic_string<CharT>::iterator it= src.begin();
  while(true)
  {
    it= user::search(src.begin(), src.end(), target.begin(), target.end());
    if(it==src.end())
       return;

    src.erase(it, it+ target.size());
    size_t d= it-src.begin();
    src.insert(it, replacement.begin(), replacement.end());
    it= src.begin()+d+replacement.size();
  }
}
//---------------------------------------------------------------------
template<typename StringT>
Vector<StringT>  split(const StringT& str,const StringT& separator)
{
  Vector<StringT> r;
  size_t start=0;
  while(start<str.size())
  {
    size_t pos= str.find(separator,start);
    if(pos==StringT::npos)
      pos = str.size();

    const StringT part = str.substr(start,pos-start);
    if(!part.empty())
       r.push_back(part);

    start = pos+separator.size();
  }
  return r;
}
//---------------------------------------------------------------------
String extractParamValue(const String& url,const String& paramName)
{
   size_t paramNamePos= url.find(paramName+"=");
   if(paramNamePos==String::npos)
     return String();

   size_t startOfValue= paramNamePos+paramName.size()+1;
   size_t endOfValue=   url.find("&",startOfValue);
   if(endOfValue==String::npos)
      endOfValue= url.size();

   return url.substr(startOfValue,endOfValue-startOfValue);
}
//---------------------------------------------------------------------
HttpServer::HttpServer(Storage *storage)
   :storage_(storage),
    page_(1)
{

}
//---------------------------------------------------------------------
void HttpServer::start(unsigned short port)
{
   setAddr("127.0.0.1");
   setPort(port);
   bind();
   listen();
   accept();
}
//---------------------------------------------------------------------
void HttpServer::workWithClient(TcpClient *client)
{
  try
  {
    String requestData(bufferSize_);
    client->read(requestData.data(),requestData.size());
    size_t endOfHeaders = requestData.find("\r\n\r\n");
    if(endOfHeaders!=String::npos)
      requestData.resize(endOfHeaders);

    if(parseRequestParams(requestData) || url_=="/" || url_.empty())
    {
       routeMain(client);
    }
    else
    {
       WString fileName= join(storage_->dir(),bytesToWString(percentDecode(url_),CP_UTF8));
       stringReplace<wchar_t>(fileName,L"/",L"\\");
       routeFile(client,fileName);
    }
  }
  catch (const WsaException& e)
  {
    printf("Client error: %s\n",wStringToBytes(e.message_w(),CP_UTF8).c_str());
  }
}
//---------------------------------------------------------------------
bool HttpServer::parseRequestParams(const String &headers)
{
   const Strings lines = split(headers,String("\r\n"));
   url_.clear();

   for(const String& line: lines)
   {
      const size_t getPos = line.find("GET ");
      if(getPos==0)
      {
         const size_t httpPos = line.find(" HTTP",getPos);
         if(httpPos!=String::npos)
         {
            url_ = line.substr(getPos+4,httpPos-(getPos+4));
            const String query= extractParamValue(url_,"query");
            if(!query.empty())
            {
              const String percentDecodedQuery= percentDecode(query);
              queryString_= bytesToWString(percentDecodedQuery,CP_UTF8);

              const String pageStr= extractParamValue(url_,"page");
              if(!tryStrToUInt(pageStr,page_) || page_<1)
                 page_= 1;

              return true;
            }
         }
      }
   }
   queryString_.clear();
   page_= 1;
   return false;
}
//---------------------------------------------------------------------
void HttpServer::routeMain(TcpClient *client)
{
   String templateContent = readAllText(L"template.html");
   templateContent.reserve(templateContent.size()*2/3);
   stringReplace<char>(templateContent,"{{query_str}}",wStringToBytes(queryString_,CP_UTF8));

   if(queryString_.empty())
   {
     stringReplace<char>(templateContent,"{{message}}","");
     stringReplace<char>(templateContent,"{{items}}","");
     stringReplace<char>(templateContent,"{{visibility}}","hidden");
   }
   else
   {     
     Query query(storage_,queryString_);
     if(query.isValid())
     {
       printf("Запрос: '%s'\n",wStringToBytes(queryString_,CP_UTF8).c_str());
       const unsigned long startTime= GetTickCount();

       Locations locations = query.prepare();

       printf("Время поиска: %lu мс\n",GetTickCount()-startTime);

       const size_t itemCount= 50;
       const size_t firstItem= (page_-1)*itemCount;
       const size_t lastItem=  user::min_(page_*itemCount,locations.size());

       String itemsHtml;
       itemsHtml.reserve(300*locations.size());

       for(size_t i=firstItem; i<lastItem; ++i)
       {
         const WString fileName= storage_->fileNames()[locations[i].fileIndex];
         const WString aValue=   baseName(fileName);
         const String  snippet=  wStringToBytes(storage_->snippets()[locations[i].fileIndex],CP_UTF8);
         itemsHtml+= "<div class='item'>"
                           "<a class='link' href='"+wStringToBytes(fileName,CP_UTF8)+"'><h3>"+
                                       uIntToStr(i+1)+". "+wStringToBytes(aValue,CP_UTF8)+
                           "</h3></a>"
                           "<span class='snippet'>"+snippet+"</span>"
                      "</div>";
       }

       if(locations.empty())
       {
          stringReplace<char>(templateContent,"{{message}}","Ничего не найдено!");
          stringReplace<char>(templateContent,"{{message_color}}","#ffa812");
       }
       else
       {
          stringReplace<char>(templateContent,"{{message}}",
                              "Найдено документов: "+uIntToStr(locations.size()));
          stringReplace<char>(templateContent,"{{message_color}}","lime");
       }
       stringReplace<char>(templateContent,"{{items}}",itemsHtml);

       const size_t nextPage=  (page_*itemCount < locations.size()) ? page_+1: page_;
       stringReplace<char>(templateContent,"{{visibility}}",nextPage!=page_?"visible":"hidden");
       stringReplace<char>(templateContent,"{{next_page}}",uIntToStr(nextPage));
       const String nextPageUrl =
           "/?query="+wStringToBytes(queryString_,CP_UTF8)+
            "&page=" +uIntToStr(nextPage);
       stringReplace<char>(templateContent,"{{next_page_url}}",nextPageUrl);
     }
     else
     {
       stringReplace<char>(templateContent,"{{message}}","Не корректный запрос!");
       stringReplace<char>(templateContent,"{{message_color}}","red");
       stringReplace<char>(templateContent,"{{items}}","");
       stringReplace<char>(templateContent,"{{visibility}}","hidden");
     }
   }
   writeOkResponseHeaders(client,templateContent.size());
   client->write(templateContent.data(),templateContent.size());
}
//---------------------------------------------------------------------
void HttpServer::routeFile(TcpClient *client, const WString &filePath)
{
  if(exists(filePath))
  {
    const String content = readAllText(filePath);
    writeOkResponseHeaders(client,content.size());
    client->write(content.data(),content.size());
  }
  else
  {
    const String content = "<h1><span style='color:red'>404. Файл не найден!</span></h1>";
    write404ResponseHeaders(client,content.size());
    client->write(content.data(),content.size());
  }
}
//---------------------------------------------------------------------
void HttpServer::writeOkResponseHeaders(TcpClient *client, size_t contentLength)
{
  const String headers=
      "HTTP/2 200 OK\r\n"
      "Content-Language: ru\r\n"
      "Content-Type: text/html; charset=utf-8\r\n"
      "Connection: close\r\n";

  client->write(headers.data(),headers.size());
  const String contentLengthHeader= "Content-Length: "+uIntToStr(contentLength)+"\r\n\r\n";
  client->write(contentLengthHeader.data(),contentLengthHeader.size());
}
//---------------------------------------------------------------------
void HttpServer::write404ResponseHeaders(TcpClient *client, size_t contentLength)
{
  const String headers=
      "HTTP/2 404 Not Found\r\n"
      "Content-Language: ru\r\n"
      "Content-Type: text/html; charset=utf-8\r\n"
      "Connection: close\r\n";

  client->write(headers.data(),headers.size());
  const String contentLengthHeader= "Content-Length: "+uIntToStr(contentLength)+"\r\n\r\n";
  client->write(contentLengthHeader.data(),contentLengthHeader.size());
}
//---------------------------------------------------------------------