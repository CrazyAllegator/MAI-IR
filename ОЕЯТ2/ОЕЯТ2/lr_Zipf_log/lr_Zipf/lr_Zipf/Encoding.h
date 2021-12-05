#ifndef ENCODING_H
#define ENCODING_H
//-----------------------------------------------------------
#include "Global.h"
#include "Exception.h"
//-----------------------------------------------------------
/*
 * Так как тексты на русском в UTF-8
 * для удобной работы с приходится конвертировать их в UCS-2
 * и работать с юникодом.
 * Для конвертации используем ф-ции обвертки над системными ф-циями(WinApi)
*/
//-----------------------------------------------------------
WString bytesToWString(const char* data,int size, unsigned sourceCp);
WString bytesToWString(const String& in, unsigned sourceCp);

int wStringToBytes(const WString& in,char* data,int size, unsigned targetCp);
String wStringToBytes(const WString& in, unsigned targetCp);

bool tryStrToUInt(const String& str,size_t& value);
bool tryHexStrToInt(const String& str,int& value);
String uIntToStr(size_t value);

String percentDecode(const String& text);
WString canonicalizeUrl(const WString &text);
//-----------------------------------------------------------
#endif // ENCODING_H
