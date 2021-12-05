#include "Exception.h"
//---------------------------------------------------------------------------
// Форматирование сообщения об ошибки по коду ошибки из WiApi-ф-ций
//---------------------------------------------------------------------------
String formatMessage(unsigned long code, const  WString &moduleName=WString())
{
  bool useModule= !moduleName.empty();
  HMODULE module = useModule?GetModuleHandle(moduleName.c_str()):nullptr;

  LPSTR errorMsg = nullptr;   // will be allocated and filled by FormatMessage
  unsigned long r= FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                (useModule?FORMAT_MESSAGE_FROM_HMODULE:0) |
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_ARGUMENT_ARRAY, // use windows internal message table
             module,       // 0 since source is internal message table
             code,    // this is the error code
             // Could just as well have been an error code from generic
             // Windows errors from GetLastError()
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // auto-determine language to use
             reinterpret_cast<LPSTR>(&errorMsg),
             0,       // min length for buffer
             nullptr);

  return r?errorMsg:"Can not get error code description!";
}
//---------------------------------------------------------------------------
WString  formatMessageW(unsigned long code, const  WString &moduleName=WString())
{
  bool useModule= !moduleName.empty();
  HMODULE module = useModule?GetModuleHandle(moduleName.c_str()):nullptr;

  LPTSTR errorMsg = nullptr;   // will be allocated and filled by FormatMessage
  unsigned long r= FormatMessage(
             FORMAT_MESSAGE_ALLOCATE_BUFFER |
             FORMAT_MESSAGE_FROM_SYSTEM |
             (useModule?FORMAT_MESSAGE_FROM_HMODULE:0) |
             FORMAT_MESSAGE_ALLOCATE_BUFFER |
             FORMAT_MESSAGE_IGNORE_INSERTS |
             FORMAT_MESSAGE_ARGUMENT_ARRAY, // use windows internal message table
             module,       // 0 since source is internal message table
             code,    // this is the error code
             // Could just as well have been an error code from generic
             // Windows errors from GetLastError()
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // auto-determine language to use
             reinterpret_cast<LPTSTR>(&errorMsg),
             0,       // min length for buffer
             nullptr);

  return r?errorMsg:L"Не удалось получить описание ошибки.";
}
//---------------------------------------------------------------------------
WString appModuleFileName()
{
  WString filePath(MAX_PATH,L'\0');
  unsigned long length= GetModuleFileName(nullptr, &filePath[0], filePath.size());
  filePath.resize(length);
  return filePath;
}
//---------------------------------------------------------------------------
Exception::Exception(const char *message)
  :message_(message)
{

}
//---------------------------------------------------------------------------
String Exception::message() const
{
  return message_;
}
//---------------------------------------------------------------------------
SystemException::SystemException()
{
  code_= GetLastError();
}
//---------------------------------------------------------------------------
SystemException::SystemException(unsigned long errorCode)
  : code_(errorCode)
{

}
//---------------------------------------------------------------------------
String SystemException::message() const
{
  return formatMessage(code_);
}
//---------------------------------------------------------------------------
WString SystemException::message_w() const
{
  return formatMessageW(code_);
}
//---------------------------------------------------------------------------
unsigned long SystemException::code() const
{
  return code_;
}
