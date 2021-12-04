#include "Tokenizing.h"

/*
 * Так как тексты статей на русском берем только русские слова
 * остальные символы (цифры,латиницу,греческий,специфические юникод символы) игнорируем
 * и считаем их разделителями.
 *
 * Та как как исходная кодировка файлов UTF-8 - многобайтовая
 * мы ее переводим UCS-2 (два байта) для удобства анализа.
 * Создаем свою ф-ции для определения является ли символ кириллическим и для понижения регистра
 * руководствуюясь таблицами юникод символов:
 * https://unicode-table.com/ru/blocks/cyrillic/
 */


bool isCyrilic(wchar_t c)
{
  //       А        ...    я         Ё               ё
  return (c>=0x0410 && c<=0x044F) || c==0x0401 || c== 0x0451;
}

wchar_t lowelCase(wchar_t c)
{
  //   А              Я
  if(c>=0x0410 && c<=0x042F)
    return c+0x0020;
  else if(c==0x0401) // Ё
    return 0x0451;   // ё

  return c;
}

void lower(WString& wstr)
{
  for(size_t i=0; i<wstr.size(); ++i)
    wstr[i]= lowelCase(wstr[i]);
}


Tokens tokenize(const WString &text)
{
  Tokens tokens;  
  WString currentToken;
  size_t tokenNumber= 1;
  //size_t line= 1;
  //size_t posInLine= 1;
  for(size_t i=0; i<text.size(); ++i)
  {
    if(isCyrilic(text[i]))
    {
        currentToken +=  lowelCase(text[i]);
    }
    else
    {
      if(!currentToken.empty())
      {
        TokenInfo token;
        token.token = currentToken;
        token.location.tokenNumber = tokenNumber;
        token.location.pos = i-currentToken.size();
        //token.location.posInLine = posInLine-currentToken.size();
        //token.location.line = line;
        tokens.push_back(token);
        currentToken.clear();
        ++tokenNumber;
      }
//      if(text[i]==L'\n')
//      {
//        ++line;
//        posInLine= 0;
//      }
    }
    //++posInLine;
  }

  if(!currentToken.empty())
  {
    TokenInfo token;
    token.token = currentToken;
    token.location.tokenNumber = tokenNumber;
    token.location.pos = text.size()-currentToken.size();
    //token.location.posInLine = posInLine-currentToken.size();
    //token.location.line = line;
    tokens.push_back(token);
  }
  return tokens;
}
