#include "FS.h"
// ----------------------------------------------------
bool isDot(const WString& dir)
{
  return dir== L"." || dir==L"..";
}
// ----------------------------------------------------
bool isSeparator(wchar_t ch)
{
  return ch==L'\\' || ch==L'/';
}
// ----------------------------------------------------
WString join(const WString& dir, const WString& rel)
{
  if(!dir.empty() && !rel.empty() &&
     !isSeparator(dir[dir.size()-1]) && !isSeparator(rel[0]))
    return dir+L"\\"+rel;
  return dir+rel;
}
// ----------------------------------------------------
WString join(const WString& dir, const WString& rel1, const WString& rel2)
{
  return join(join(dir,rel1),rel2);
}
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
// ----------------------------------------------------
WString baseName(const WString &fileName)
{
  int extPos= fileName.size();
  for(int i=fileName.size(); i>=0; --i)
  {
    if(fileName[i]=='.' && extPos==static_cast<int>(fileName.size()))
      extPos= i;

    if(isSeparator(fileName[i]))
       return fileName.substr(i+1,extPos-(i+1));
  }
  return fileName;
}
// ----------------------------------------------------
size_t entry(WStrings &entries,
             const WString &dir,
             const WString &mask,
             EntryFlags flags,
             const WString &subDir)
{
  // Seach by mask
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(join(dir,subDir,mask).c_str(), &data);
    if(handle!=INVALID_HANDLE_VALUE)
    {
      do
      {
        if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // is Dir
        {
          if((flags & Dirs) && (!isDot(data.cFileName) || (flags & Dots)))
            entries.push_back(join(subDir,data.cFileName));
        }
        else if(flags & Files) // is File or some else
          entries.push_back(join(subDir,data.cFileName));
      }
      while(FindNextFile(handle,&data)!=0);
      FindClose(handle);
    }
    // Recursive search in sub dirs
    if(flags & Recursive)
    {
      WStrings subDirs;
      entry(subDirs, join(dir,subDir),L"*", Dirs);
      for(size_t i=0; i<subDirs.size(); ++i)
        entry(entries,dir,mask,flags,join(subDir,subDirs[i]));
    }
    return entries.size();
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
//---------------------------------------------------------------------------
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365541(v=vs.85).aspx
//---------------------------------------------------------------------------
unsigned long File::setFilePointer(long  distanceToMove,
	unsigned long moveMethod,
	long* distanceToMoveHigh/* =0 */)
{
	unsigned long newFilePointer =
		SetFilePointer(handle_,
			distanceToMove,
			distanceToMoveHigh,
			moveMethod);

	/* ѕоследний аргумент moveMethod указывает,
	   откуда будет начат отсчет, принимает значени€:
	   FILE_BEGIN - начало файла,
	   FILE_CURRENT - с текущего значени€,
	   FILE_END - с конца файла.
	 */

	if (distanceToMoveHigh)
	{
		unsigned long errorCode = GetLastError();
		if (newFilePointer == INVALID_SET_FILE_POINTER && errorCode != NO_ERROR)
			throw SystemException(errorCode);
	}
	else
	{
		if (newFilePointer == INVALID_SET_FILE_POINTER)
			throw SystemException();
	}
	return newFilePointer;
}
//---------------------------------------------------------------------------
__int64 File::seek(__int64 distance, unsigned long moveMethod)
{
	LARGE_INTEGER li;
	li.QuadPart = distance;
	li.LowPart = setFilePointer(li.LowPart, moveMethod, &li.HighPart);
	return li.QuadPart;
}
//---------------------------------------------------------------------------
