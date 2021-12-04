#ifndef GLOBAL_H
#define GLOBAL_H
//-----------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
//-----------------------------------------------------------
#ifdef _DEBUG
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "UserVector.h"
#include "UserString.h"
#include "Utils.h"

template<typename T> using Vector= user::vector<T>;
typedef user::basic_string<char> String;
typedef user::basic_string<wchar_t> WString;
template<typename T1,typename T2> using Pair= user::pair<T1,T2>;

typedef Vector<String>  Strings;
typedef Vector<WString> WStrings;
//--------------------------------------------------------------
struct Location
{
    size_t fileIndex;

    size_t tokenNumber;
    size_t pos;

    explicit Location(size_t fileIndex=0,size_t tokenNumber=0,size_t pos=0)
      :fileIndex(fileIndex),tokenNumber(tokenNumber),pos(pos)
    {
    }

    bool operator==(const Location& other)const
    {
      return fileIndex==other.fileIndex;
    }

    bool operator!=(const Location& other)const
    {
      return fileIndex!=other.fileIndex;
    }

    bool operator<(const Location& other)const
    {
      return fileIndex<other.fileIndex;
    }

    bool operator>(const Location& other)const
    {
      return fileIndex>other.fileIndex;
    }
};
//--------------------------------------------------------------
struct TokenInfo
{
   WString token;
   Location location;
};
//--------------------------------------------------------------
typedef Vector<TokenInfo> Tokens;
typedef Vector<Location>  Locations;
//-----------------------------------------------------------
#endif // GLOBAL_H
