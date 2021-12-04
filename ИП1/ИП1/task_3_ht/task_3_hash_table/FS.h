#ifndef FS_H
#define FS_H
//---------------------------------------------------------------
#include "Exception.h"
#include "Global.h"

#include <Windows.h>
#include <Shlwapi.h>
//---------------------------------------------------------------
/*
* Для работы с файлами используем самописные
* обвертки над системными ф-циями(WinApi)
* (При отстутствии ограничений можно было бы воспользоваться
*  std::filesystem или boost::filesystem, а еще лучше Qt)
*/
//---------------------------------------------------------------
// Флаги поиска
enum EntryFlags
{
    Files = 1,
    Dirs  = 2,
    Dots = 4,
    Recursive= 8,

    Entries = Dirs|Files,

    AllFiles=  Files|Recursive,
    AllDirs=   Dirs|Recursive,
    AllEntries= Files|Dirs|Recursive
};
//---------------------------------------------------------------
// Поиск файлов в папке
size_t entry(WStrings& entries,
             const WString& dir,
             const WString& mask= WString(L"*"),
             EntryFlags flags= Entries,
             const WString& subDir= WString());
//---------------------------------------------------------------
// Объединение путей
WString join(const WString& dir, const WString& rel);
WString join(const WString& dir, const WString& rel1, const WString& rel2);
//---------------------------------------------------------------
// Полный(абсолютный) путь
WString fullPathName(const WString& path);
//---------------------------------------------------------------
// Файл/папка существует
bool exists(const WString &path);
//---------------------------------------------------------------
// Читать файл полностью
Bytes readAllText(const WString& fileName);
//---------------------------------------------------------------
// Класс файла для чтения и записи
//---------------------------------------------------------------
class File
{
   public:
      File();
      ~File();
      void close();

      // write GENERIC_WRITE, CREATE_ALWAYS
      // read  GENERIC_READ,  OPEN_EXISTING

      void open(const WString& fileName,
                unsigned long desiredAccess= GENERIC_READ|GENERIC_WRITE,
                unsigned long creationDisposition= CREATE_NEW);

      unsigned long read(void* data,size_t size);
      unsigned long write(const void* data,size_t size);

      void flush();

      // Класс нельзя копировать
      File(const File& file)=delete;
      File& operator=(const File& file)=delete;
   private:
      HANDLE handle_;
};
//---------------------------------------------------------------
void consolePrint(const WString& text); // Вывод utf16 в консоль
//---------------------------------------------------------------
#endif // FS_H
