#include <stdio.h>
#include <new>      // only for catch std::bad_alloc

#include "Global.h"
#include "Encoding.h"
#include "FS.h"
#include "Tokenizing.h"

bool parseArgs(int argc, char *argv[],
               WString& fileName)
{
  if(argc==3)
  {
    String key(argv[1]);
	String value(argv[2]);
    if(key=="-fn" || key=="--file_name")
    {
       fileName = fullPathName(bytesToWString(value,CP_UTF8));
	   return exists(fileName);
    }
  }
  return false;
}

int main(int argc, char *argv[])
{
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  WString fileName;
  if(!parseArgs(argc, argv, fileName))
  {
    printf("Не верно указан входной файл!\n");
    return 1;
  }

  try
  {
   const String  bytes= readAllText(fileName);
   const WString text=  bytesToWString(bytes,CP_UTF8);

   LARGE_INTEGER frequency, start, finish;
   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&start);

   Tokens tokens= tokenize(text);

   QueryPerformanceCounter(&finish);   
   const float totalDuration = (finish.QuadPart - start.QuadPart) * 1000. / frequency.QuadPart; // ms

   size_t totalTokenLength=0;
   for(size_t t=0; t<tokens.size(); ++t)
   {
     totalTokenLength+= tokens[t].token.length();
     printf("Token: \"%s\"\n", wStringToBytes(tokens[t].token,CP_UTF8).c_str());
     printf("  Position: %u\n",tokens[t].location.pos);
     printf("  Number: %u\n",tokens[t].location.tokenNumber);
   }

   printf("=================================\n");
   printf("Bytes: %u B\n",  bytes.size());
   printf("Duration: %f ms\n",  totalDuration);
   printf("Term count: %u\n", tokens.size());
   printf("Mean term length: %7.2f\n",  totalTokenLength / float(tokens.size()) );
   printf("Speed: %7.2f kB/s\n",    bytes.size()/1000. / (totalDuration/1000.) );
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

  // При отладочной сборке проверяем отладчиком наличие утечек памяти
  #ifdef _DEBUG
    _CrtDumpMemoryLeaks();
  #endif

  return 0;
}
