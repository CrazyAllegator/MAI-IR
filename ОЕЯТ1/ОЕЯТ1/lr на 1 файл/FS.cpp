#include "FS.h"

#include <shlwapi.h>
// ----------------------------------------------------
WString fullPathName(const WString& path)
{
  unsigned long length = GetFullPathName(path.c_str(), 0, nullptr, nullptr);
	if (length == 0)
    throw SystemException();

  WString buffer(length,L'\0');

  length = GetFullPathName(path.c_str(), buffer.size(),
                           const_cast<wchar_t*>(buffer.data()), nullptr);
	if (length == 0)
    throw SystemException();

	buffer.resize(length);
	return buffer;
}
// ----------------------------------------------------
bool exists(const WString &path)
{
  return PathFileExists(path.c_str()) == TRUE;
}
//--------------------------------------------------------------------------------------------
String readAllText(const WString& fileName)
{
   WIN32_FILE_ATTRIBUTE_DATA data;
   data.nFileSizeLow =  0;
   data.nFileSizeHigh = 0;

   if(!GetFileAttributesEx(fileName.c_str(),
                           GetFileExInfoStandard,
                           reinterpret_cast<void*>(&data)))
      throw SystemException();

   if(data.nFileSizeHigh!=0)
      throw Exception("File length to long!");

   String buffer(data.nFileSizeLow);
   File file;
   file.open(fileName, GENERIC_READ, OPEN_EXISTING);

   unsigned long length= file.read(const_cast<char*>(buffer.data()),
                                   buffer.size());
   buffer.resize(length);

  return buffer;
}
//--------------------------------------------------------------------------------------------
File::File()
   :handle_(INVALID_HANDLE_VALUE)
{

}
//--------------------------------------------------------------------------------------------
File::~File()
{
  close();
}
//--------------------------------------------------------------------------------------------
void File::close()
{
  if(handle_!= INVALID_HANDLE_VALUE)
  {
    CloseHandle(handle_);
    handle_= INVALID_HANDLE_VALUE;
  }
}
//--------------------------------------------------------------------------------------------
void File::open(const WString &fileName,
                unsigned long desiredAccess,
                unsigned long creationDisposition)
{
  handle_ =
      CreateFile(fileName.c_str(),
                 desiredAccess,
                 0,
                 nullptr,
                 creationDisposition,
                 FILE_ATTRIBUTE_NORMAL,
                 nullptr);

  if(handle_== INVALID_HANDLE_VALUE)
    throw SystemException();
}
//--------------------------------------------------------------------------------------------
unsigned long File::read(void *data, size_t size)
{
  unsigned long length= 0;
  BOOL success= ReadFile(handle_,
                         data, size,
                         &length, nullptr);
  if(!success)
    throw SystemException();
  return length;
}
//--------------------------------------------------------------------------------------------
unsigned long File::write(const void *data, size_t size)
{
  unsigned long length= 0;
  BOOL success= WriteFile(handle_,
                          data, size,
                          &length, nullptr);
  if(!success)
    throw SystemException();
  return length;
}
//--------------------------------------------------------------------------------------------
void File::flush()
{
  if(handle_== INVALID_HANDLE_VALUE)
     FlushFileBuffers(handle_);
}
//--------------------------------------------------------------------------------------------
void writeConsole(const WString& text)
{
  static HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  static unsigned long w;
  WriteConsole(consoleHandle,text.c_str(),text.size(),&w,nullptr);
}
//--------------------------------------------------------------------------------------------


