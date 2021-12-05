#ifndef GLOBAL_H
#define GLOBAL_H
//-----------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <shellapi.h>
//-----------------------------------------------------------
// В отладочной сборке проверяем программу на утечки памяти
#ifdef _DEBUG
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/*
     Далее используем псевдонимы для контейнеров
     что бы можно было проверить как работает программа на STL- контейнерах,
     и сравнить с самописными пользовательскими контейнерами.

     Для того что бы помотреть как работает программа на STL контейнерах
     нужно раскоментировать следующие три строчки (макроса-define -a)
*/


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


typedef Vector<String>  Strings;
typedef Vector<WString> WStrings;
//-----------------------------------------------------------
#endif // GLOBAL_H
