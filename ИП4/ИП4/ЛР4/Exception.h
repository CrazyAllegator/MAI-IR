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
class SystemException: public BaseException
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
class WsaException: public BaseException
{
public:
  WsaException();
  explicit WsaException(unsigned long errorCode);

  virtual unsigned long code() const;
  virtual String  message() const;
  virtual WString message_w()const;

  virtual const char *what() const;

private:
  unsigned long code_;
  String message_;
};
//---------------------------------------------------------------------------
class WsaTimeOutException: public WsaException
{
  public:
     WsaTimeOutException();
     explicit WsaTimeOutException(unsigned long errorCode);
};
//------------------------------------------------------------------
#endif // EXCEPTION_H
