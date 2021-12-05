#ifndef FS_H
#define FS_H
//---------------------------------------------------------------
#include "Exception.h"
#include "Global.h"

#include <Shlwapi.h>
//---------------------------------------------------------------
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
size_t entry(WStrings& entries,
             const WString& dir,
             const WString& mask= WString(L"*"),
             EntryFlags flags= Entries,
             const WString& subDir= WString());
//---------------------------------------------------------------
WString join(const WString& dir, const WString& rel);
WString join(const WString& dir, const WString& rel1, const WString& rel2);
//---------------------------------------------------------------
WString fullPathName(const WString& path);
//---------------------------------------------------------------
bool exists(const WString &path);
//---------------------------------------------------------------
String readAllText(const WString& fileName);
//---------------------------------------------------------------
WString baseName(const WString& fileName);
//---------------------------------------------------------------
class File
{
   public:
      File();
      ~File();
      void close();

      void open(const WString& fileName,
                unsigned long desiredAccess= GENERIC_READ|GENERIC_WRITE,
                unsigned long creationDisposition= CREATE_NEW);

      unsigned long read(void* data,size_t size);
      unsigned long write(const void* data,size_t size);

      void flush();

      File(const File& file)=delete;
      File& operator=(const File& file)=delete;
   private:
      HANDLE handle_;
};
//---------------------------------------------------------------
void writeConsole(const WString& text);
//---------------------------------------------------------------
#endif // FS_H
