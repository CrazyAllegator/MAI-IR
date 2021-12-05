#include "Encoding.h"
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
bool tryStrToUInt(const String& str,size_t& value)
{
  return sscanf_s(str.c_str(),"%u", &value)==1;
}
//------------------------------------------------------------------
bool tryHexStrToInt(const String& str,int& value)
{
  return sscanf_s(str.c_str(),"%x", &value)==1;
}
//------------------------------------------------------------------
String percentDecode(const String& text)
{
  String r;
  for(size_t i=0; i<text.size(); ++i)
  {
     switch(text[i])
     {
       case '%':
       {
        int value=0;
        if(tryHexStrToInt(text.substr(i+1,2),value))
          r+= (char)value;
        i+= 2;
       }
       break;

       case '+': { r+=" "; }  break;

       default:  { r+= text[i]; } break;
     }
  }
  return r;
}
//------------------------------------------------------------------
String uIntToStr(size_t value)
{
  String r(16,'\0');
  sprintf_s(r.data(),r.size(),"%u",value);
  r.resize(strlen(r.c_str()));
  return r;
}
//------------------------------------------------------------------
