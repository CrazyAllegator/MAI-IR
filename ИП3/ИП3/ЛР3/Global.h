#ifndef GLOBAL_H
#define GLOBAL_H
//-----------------------------------------------------------
#ifdef _DEBUG
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//#define USE_STL_VECTOR
//#define USE_STL_STRING
//#define USE_STL_LIST
//#define USE_STL_PAIR

// VECTOR
#ifdef USE_STL_VECTOR
  #include <vector>
  template<typename T> using Vector= std::vector<T,std::allocator<T> >;
#else
  #include "UserVector.h"
  template<typename T> using Vector= user::vector<T>;
#endif

// STRING
#ifdef USE_STL_STRING
  #include <string>
  typedef std::wstring WString;
  typedef std::string String;
#else
  #include "UserString.h"
  typedef user::basic_string<char> String;
  typedef user::basic_string<wchar_t> WString;
#endif

// LIST
#ifdef USE_STL_LIST
   #include<list>
   template<typename T> using List= std::list<T,std::allocator<T> >;
#else
   #include "UserList.h"
   template<typename T> using List= user::list<T>;
#endif

#ifdef USE_STL_PAIR
   #include  <utility>
   template<typename T1,typename T2> using Pair= std::pair<T1,T2>;
#else
   #include "Utils.h"
   template<typename T1,typename T2> using Pair= user::pair<T1,T2>;
#endif

typedef Vector<String>  Strings;
typedef Vector<WString> WStrings;
typedef Vector<char> Bytes;
//-----------------------------------------------------------
#endif // GLOBAL_H
