#include <cstdio> // for printf
#include <new>    // only for catch std::bad_alloc

#include "Global.h"

#include "Storage.h"
#include "Exception.h"
#include "Encoding.h"
//---------------------------------------------------------------------------------------------
enum class Mode { index, load };
//---------------------------------------------------------------------------------------------
bool parseArgs(int argc, char *argv[],
               WString& dir,
               size_t& bucketSize,
               size_t& maxFileCount,
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
         sscanf(args[i].c_str(), "%d", &bucketSize)!=1 ||
         bucketSize==0)
        return false;
    }
    else if(args[i]=="--max_file_count")
    {
      if(++i>=args.size() ||
         sscanf(args[i].c_str(), "%d", &maxFileCount)!=1)
        return false;
    }
    else if(args[i]=="--mode")
    {
      if(++i<args.size())
      {
        if(args[i]=="load")
           mode = Mode::load;
        else
           mode = Mode::index;
      }
      else
        return false;
    }
  }
  return true;
}
//---------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // меняем кодировку консоли c cp866 на utf8
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
  try
  {
    WString dir= LR"(..\..\docs)";  // по умолчанию файлы ищутся на два уровня выше в папке docs
    size_t bucketSize = 400'000; // в словоре Даля вроде как 200 000 слов, но у нас думаю значительно меньше
    size_t maxFileCount = 0; // 0 - нет ограничения
    Mode mode = Mode::index;
    if(!parseArgs(argc, argv, dir, bucketSize, maxFileCount, mode))  // парсим аргументы консоли
      throw Exception("Агрументы консоли заданы не верное!");

    dir = fullPathName(dir); // получаем полный путь
    if(!exists(dir) && mode == Mode::index)
      throw Exception("Указан не верный пусть к папке с документами!");

    Storage storage(bucketSize);
    if(mode==Mode::index) // Создание индекса и сохранение
    {
      storage.index(dir,maxFileCount); // анализируем данные поиск файлов/разбор на токены.
      storage.save(L"index.binary"); // сохраняем в бинарный файл
    }
    else
    {
      storage.load(L"index.binary"); // загрузка из файла
      storage.printFileNames();
      storage.printSnippets();
    }
    // Вывод в консоль токенов:
    storage.printTable();
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

  //system("pause");
  return 0;
}
