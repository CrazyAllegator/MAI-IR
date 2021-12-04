#ifndef ENCODING_H
#define ENCODING_H
//-----------------------------------------------------------
#include "Global.h"
//-----------------------------------------------------------
WString bytesToWString(const char* data,int size, unsigned sourceCp);
WString bytesToWString(const Bytes& in, unsigned sourceCp);
WString bytesToWString(const String& in, unsigned sourceCp);

int wStringToBytes(const WString& in,char* data,int size, unsigned targetCp);
String wStringToBytes(const WString& in, unsigned targetCp);
//-----------------------------------------------------------
#endif // ENCODING_H
