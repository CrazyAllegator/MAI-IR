#include "Tokenizing.h"

bool isCyrilic(wchar_t c)
{
  return (c>=0x0410 && c<=0x044F) || c==0x0401 || c== 0x0451;
}

wchar_t lowelCase(wchar_t c)
{
  if(c>=0x0410 && c<=0x042F)
    return c+0x0020;
  else if(c==0x0401)   // Ё
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
        tokens.push_back(token);
        currentToken.clear();
        ++tokenNumber;
      }
    }
  }

  if(!currentToken.empty())
  {
    TokenInfo token;
    token.token = currentToken;
    token.location.tokenNumber = tokenNumber;
    token.location.pos = text.size()-currentToken.size();
    tokens.push_back(token);
  }
  return tokens;
}
