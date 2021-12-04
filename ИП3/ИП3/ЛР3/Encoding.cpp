#include <Windows.h>

#include "Encoding.h"
#include "Exception.h"

//------------------------------------------------------------------
WString bytesToWString(const char*data, int size, unsigned sourceCp)
{
    if(size==0)
      return WString();

    int length =
        MultiByteToWideChar(sourceCp, 0,
                            data, size,
                            nullptr, 0);

    if(length==0)
       throw SystemException();

    WString buffer(static_cast<size_t>(length),L'\0');

    length= MultiByteToWideChar(sourceCp, 0,
                        data, size,
                        const_cast<wchar_t*>(buffer.data()),
                        static_cast<int>(buffer.size()) );
    if(length==0)
       throw SystemException();

    buffer.resize(static_cast<size_t>(length));
    return  buffer;
}

//------------------------------------------------------------------
WString bytesToWString(const Bytes &in, unsigned sourceCp)
{
  return bytesToWString( reinterpret_cast<const char*>(in.data()),
                         static_cast<int>(in.size()),
                         sourceCp);
}
//------------------------------------------------------------------
WString bytesToWString(const String &in, unsigned sourceCp)
{
  return bytesToWString(in.data(),in.size(),sourceCp);
}

//------------------------------------------------------------------
int wStringToBytes(const WString& in, char *data, int size, unsigned targetCp)
{  
  int length = WideCharToMultiByte(targetCp, 0,
                                   in.data(),  static_cast<int>(in.size()),
                                   data, size,
                                   nullptr, nullptr);
  return  length;
}
//------------------------------------------------------------------
String wStringToBytes(const WString &in, unsigned targetCp)
{
  if(in.empty())
     return String();

  int length = wStringToBytes(in,
                              nullptr,0,
                              targetCp);
  if(length==0)
       throw SystemException();

  String str(static_cast<size_t>(length),'\0');

  length = wStringToBytes(in,
                          const_cast<char*>(str.data()),
                          static_cast<int>(str.size()),
                          targetCp);

  if(length==0)
       throw SystemException();

  str.resize(static_cast<size_t>(length));
  return str;
}
//------------------------------------------------------------------
