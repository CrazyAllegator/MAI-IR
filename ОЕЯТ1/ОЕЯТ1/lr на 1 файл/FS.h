#ifndef FS_H
#define FS_H
//---------------------------------------------------------------
#include "Exception.h"
#include "Global.h"
//---------------------------------------------------------------
/*
* Для работы с файлами используем самописные
* обвертки над системными ф-циями(WinApi)
* (При отстутствии ограничений можно было бы воспользоваться
*  std::filesystem или boost::filesystem, а еще лучше Qt)
*/
//---------------------------------------------------------------
// Полный(абсолютный) путь
WString fullPathName(const WString& path);
//---------------------------------------------------------------
// Файл/папка существует
bool exists(const WString &path);
//---------------------------------------------------------------
// Читать файл полностью
String readAllText(const WString& fileName);
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
#endif // FS_H
