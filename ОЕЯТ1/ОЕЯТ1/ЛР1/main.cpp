#include <stdio.h>
#include <new>

#include "Global.h"
#include "Encoding.h"
#include "FS.h"
#include "Tokenizing.h"

bool parseArgs(int argc, char *argv[],
               WString& dir)
{
  if(argc==3)
  {
    String key(argv[1]);
    String value(argv[2]);
    if(key=="-d" || key=="--dir")
    {
      dir = fullPathName(bytesToWString(value,CP_UTF8));
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[])
{
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  WString dir= LR"(..\..\docs)";
  if(!parseArgs(argc, argv, dir) && !exists(dir))
  {
    printf("Не верно указан входной файл!\n");
    return 1;
  }

  try
  {
    printf("Поиск файлов ...\n");
    WStrings fileNames;
    entry(fileNames,dir,L"*.txt",AllFiles);

    unsigned long totalTokenCount = 0;
    unsigned long totalDuration = 0;
    unsigned long totalTokenLength= 0;
    unsigned long totalBytes = 0;

    printf("Обработка файлов ...\n");
    unsigned long startTime=  GetTickCount();
    for(size_t f=0; f<fileNames.size(); ++f)
    {
	  const String fileName = wStringToBytes(fileNames[f], CP_UTF8);
      printf("file:\"%s\"\n", fileName.c_str());

      const WString path=  join(dir,fileNames[f]);
      const String  bytes= readAllText(path);
      totalBytes += bytes.size();

      const WString text=  bytesToWString(bytes,CP_UTF8);
      Tokens tokens= tokenize(text);
      totalTokenCount += tokens.size();

      for(size_t t=0; t<tokens.size(); ++t)
      {
        totalTokenLength+= tokens[t].token.length();
        tokens[t].location.fileIndex= f;
      }
    }
    totalDuration = GetTickCount() - startTime;

    printf("============== REPORT ==============\n");
    printf("Duration: %lu ms\n",  totalDuration );
    printf("Bytes: %lu B\n\n",    totalBytes );

    printf("Term count: %lu\n",       totalTokenCount);
    printf("Mean term length: %7.2f\n",  totalTokenLength / float(totalTokenCount) );

    printf("Speed: %7.2f doc/s\n",      fileNames.size() / (totalDuration/1000.)  );
    printf("Speed: %7.2f kB/s\n",    totalBytes/1000. / (totalDuration/1000.) );
    printf("====================================\n");
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
