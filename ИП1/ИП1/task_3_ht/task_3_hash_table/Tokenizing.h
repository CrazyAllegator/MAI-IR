#ifndef TOKENIZING_H
#define TOKENIZING_H
//--------------------------------------------------------------
#include "Global.h"
//--------------------------------------------------------------
/*
 * Разбиение текста на токены
 * Location - структура с информацией где обнаружен токен
 * (индекс файла в списке файлов, номер токена в файле по порядку,
 *  номер строки и номер символа в строке)
 */
//--------------------------------------------------------------
struct Location
{
    size_t fileIndex;

    size_t tokenNumber;
    size_t pos;

    //size_t line;
    //size_t posInLine;
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
