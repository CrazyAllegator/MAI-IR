#ifndef TOKENIZING_H
#define TOKENIZING_H
//--------------------------------------------------------------
#include "Global.h"
//--------------------------------------------------------------
struct Location
{
    size_t fileIndex;

    size_t tokenNumber;
    size_t pos;
};

struct TokenInfo
{
   WString token;
   Location location;
};

typedef Vector<TokenInfo> Tokens;

Tokens tokenize(const WString &text);
//--------------------------------------------------------------
#endif // FILETOKENIZER_H
