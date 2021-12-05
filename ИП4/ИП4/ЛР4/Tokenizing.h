#ifndef TOKENIZING_H
#define TOKENIZING_H
//--------------------------------------------------------------
#include "Global.h"
//--------------------------------------------------------------
Tokens tokenize(const WString &text);
bool   isCyrilic(wchar_t c);
void lower(WString& wstr);
//--------------------------------------------------------------
#endif // FILETOKENIZER_H
