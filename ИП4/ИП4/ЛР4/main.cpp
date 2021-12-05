#include <stdio.h>
#include <new>
//--------------------------------------------------------------------------
#include "Global.h"
#include "Storage.h"
#include "Exception.h"
#include "Encoding.h"
#include "Query.h"
#include "HttpServer.h"
//---------------------------------------------------------------------------
enum class Mode { index, load, query, server };
//---------------------------------------------------------------------------
bool parseArgs(int argc, char *argv[],
               WString& dir,
               size_t& bucketSize,
               size_t& maxFileCount,
               String& queryFileName,
               Mode& mode);
//---------------------------------------------------------------------------
void indexMode(Storage& storage,const WString& fileName,size_t maxFileCount);
void loadMode(Storage& storage,const WString& fileName);
void queryMode(Storage& storage, const WString& indexFileName, const String &queryFileName);
void serverMode(Storage& storage,const WString& fileName,unsigned short port);
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
  try
  {
    WString dir= LR"(..\..\docs)";
    WString indexFileName =L"index.binary";
    String  queryFileName ="queries.txt";
    size_t bucketSize = 400'000;
    size_t maxFileCount = 0;
	unsigned short port = 50'000;
    Mode mode = Mode::server;
    if(!parseArgs(argc, argv, dir, bucketSize, maxFileCount,queryFileName, mode))
      throw Exception("Агрументы консоли заданы не верное!");

    dir = fullPathName(dir);
    if(!exists(dir) && mode == Mode::index)
      throw Exception("Указан не верный пусть к папке с документами!");

    Storage storage(bucketSize);
    storage.setDir(dir);

    switch(mode)
    {
      case Mode::index:  indexMode(storage,indexFileName,maxFileCount); break;
      case Mode::load:   loadMode(storage,indexFileName); break;
      case Mode::query:  queryMode(storage,indexFileName,queryFileName); break;
      case Mode::server: serverMode(storage,indexFileName,port); break;
    }
  }
  catch (const std::bad_alloc& e)
  {
    printf("Error #1: %s \n",e.what());
  }
  catch (const Exception& e)
  {
    printf("Error #2: %s \n",e.message().c_str());
  }
  catch (const SystemException& e)
  {
    const String msg = wStringToBytes(e.message_w(),CP_UTF8);
    printf("Error #3: %s \n",msg.c_str());
  }

  #ifdef _DEBUG
    _CrtDumpMemoryLeaks();
  #endif

  return 0;
}
//---------------------------------------------------------------------------
void indexMode(Storage& storage,const WString& fileName, size_t maxFileCount)
{
  storage.index(maxFileCount);
  storage.printReport();
  storage.save(fileName);
  storage.printTable();
}
//---------------------------------------------------------------------------
void loadMode(Storage& storage,const WString& fileName)
{
  storage.load(fileName);
  storage.printFileNames();
  storage.printSnippets();
  storage.printTable();
}
//---------------------------------------------------------------------------
void queryMode(Storage& storage,
               const WString& indexFileName,
               const String& queryFileName)
{
  storage.load(indexFileName);
  Query query(&storage);
  FILE *file = fopen(queryFileName.c_str(),"r");
  if(!file)
    throw Exception("Не удается открыть входной файл c запросами!");

  const size_t maxLineLength= 1024;
  String line(maxLineLength);
  while(fgets(line.data(),line.size(),file))
  {
    line.resize(strlen(line.c_str()));
    if(!line.empty() && line[line.size()-1]=='\n')
      line.resize(line.size()-1);
    if(!line.empty() && line[line.size()-1]=='\r')
       line.resize(line.size()-1);

    if(!line.empty())
    {
       printf("Запрос: '%s'\n",line.c_str());

       const WString queryString= bytesToWString(line,CP_UTF8);
       query.setQueryString(queryString);
       if(query.isValid())
       {
           const Locations locations = query.prepare();
           printf("Найдено документов: %u\n",locations.size());

           for(size_t i=0; i<locations.size(); ++i)
           {
             const String fileName= wStringToBytes(storage.fileNames()[locations[i].fileIndex],CP_UTF8);
             const String snippet=  wStringToBytes(storage.snippets()[locations[i].fileIndex],CP_UTF8);

             printf("Файл: \"%s\"\n",fileName.c_str());
             printf("Сниппет: \"%s\"\n\n",snippet.c_str());
           }
       }
       else
       {
         printf("Не корректный запрос!\n");
       }
    }
    line.resize(maxLineLength);
  }
  fclose(file);
}
//---------------------------------------------------------------------------
void serverMode(Storage& storage,const WString& fileName,unsigned short port)
{
  storage.load(fileName);
  try
  {
    WSADATA  data;
    WSAStartup(MAKEWORD(2,0),&data);

    printf("Запуск сервера ...\n");
    HttpServer server(&storage);
    const String url="http://localhost:"+uIntToStr(port);
    ShellExecuteA(NULL,"open",url.c_str(),NULL,NULL,SW_SHOW);
    server.start(port);
  }
  catch (const WsaException& e)
  {
    const String msg = wStringToBytes(e.message_w(),CP_UTF8);
    printf("WSA Error #3: %s \n",msg.c_str());
  }
  WSACleanup();
}
//---------------------------------------------------------------------------
bool parseArgs(int argc, char *argv[],
               WString& dir,
               size_t& bucketSize,
               size_t& maxFileCount,
               String& queryFileName,
               Mode& mode)
{
  Strings args;
  for(int i=1; i<argc; ++i)
    args.push_back(argv[i]);

  for(size_t i=0; i<args.size(); ++i)
  {
    if(args[i]=="--dir")
    {
      if(++i<args.size())
         dir = bytesToWString(args[i],CP_UTF8);
      else
         return false;
    }
    else if(args[i]=="--bucket_size")
    {
      if(++i>=args.size() ||
         sscanf_s(args[i].c_str(), "%d", &bucketSize)!=1 ||
         bucketSize==0)
        return false;
    }
    else if(args[i]=="--max_file_count")
    {
      if(++i>=args.size() ||
         sscanf_s(args[i].c_str(), "%d", &maxFileCount)!=1)
        return false;
    }
    else if(args[i]=="--mode")
    {
      if(++i<args.size())
      {
        if(args[i]=="load")
           mode = Mode::load;
        else if(args[i]=="index")
           mode = Mode::index;
        else if(args[i]=="server")
           mode = Mode::server;
        else
           mode = Mode::query;
      }
      else
        return false;
    }
    else if(args[i]=="--query_file_name")
    {
      if(++i<args.size())
         queryFileName = args[i];
      else
         return false;
    }
  }
  return true;
}
//---------------------------------------------------------------------------