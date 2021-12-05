#ifndef EXCEPTION_H
#define EXCEPTION_H
//------------------------------------------------------------------
#include "Global.h"
//------------------------------------------------------------------
class BaseException
{
public:
   virtual ~BaseException(){}
   virtual String message() const=0;
};
//------------------------------------------------------------------
class Exception: public BaseException
{
public:
   explicit Exception(const char* message="");
   virtual String message() const;

private:
   String message_;
};
//------------------------------------------------------------------
class SystemException: public BaseException // Исключение по кодам ошибок WinApi
{
public:
  SystemException();
  explicit SystemException(unsigned long errorCode);
  virtual String message() const;

  virtual unsigned long code() const;
  virtual WString message_w()const;

private:
  unsigned long code_;
};
//------------------------------------------------------------------
#endif // EXCEPTION_H
