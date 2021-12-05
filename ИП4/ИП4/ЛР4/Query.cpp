#include "Query.h"
#include "Tokenizing.h"

#include "Encoding.h"
//-------------------------------------------------------------------------------
Query::Query(Storage* storage,const WString& queryStr)
 : storage_(storage),
   queryStr_(queryStr),
   index_(0),
   current_(TokenValue::NONE)
{
  lower(queryStr_);
}
//-------------------------------------------------------------------------------
void Query::setQueryString(const WString &queryStr)
{
  queryStr_ = queryStr;
  lower(queryStr_);
}
//-------------------------------------------------------------------------------
WString Query::queryString() const
{
  return queryStr_;
}
//-------------------------------------------------------------------------------
bool Query::isValid()
{
   for(wchar_t c:queryStr_)
   {
      if(!isCyrilic(c) && c!=L'&' && c!=L'|' &&  c!=L'!' && c!=L'(' &&  c!=L')' && c!=L' ')
        return false;
   }
   return true;
}
//-------------------------------------------------------------------------------
Locations Query::prepare()
{
   if(!isValid())
     return Locations();

   index_ = 0;
   current_ = TokenValue::NONE;
   locations_.clear();

   current_ = getToken();
   return level_1(false);
}
//-------------------------------------------------------------------------------
Locations Query::level_1(bool needToken) // OR
{
  Locations left = level_2(needToken);
  for(;;)
  {
    if(current_==TokenValue::OR)
       left= operationOR(left,level_2(true));
    else
       return left;
  }
}
//-------------------------------------------------------------------------------
Locations Query::level_2(bool needToken) // AND
{
  Locations left = level_3(needToken);
  for(;;)
  {
    if(current_==TokenValue::AND)
       left= operationAND(left,level_3(true));
    else
       return left;
  }
}
//-------------------------------------------------------------------------------
Locations Query::level_3(bool needToken) // NOT
{
  if(needToken)
    current_= getToken();

  switch(current_)
  {
     case TokenValue::WORD:
     {
        Locations v= locations_;
        current_= getToken();
        return v;
     };

     case TokenValue::NOT:
     {
        return operationNOT(level_3(true));
     };

     case TokenValue::LP:
     {
        Locations e = level_1(true);
        if(current_!=TokenValue::RP)
           throw Exception("Wrong query: ')' expected");
        current_ = getToken();
        return e;
     };

     default:
        throw Exception("Wrong query!");
  }
}
//-------------------------------------------------------------------------------
bool Query::get(wchar_t & ch)
{
  if(index_< queryStr_.size())
  {
    ch = queryStr_[index_++];
    return true;
  }
  return false;
}
//-------------------------------------------------------------------------------
void Query::putBack()
{
  if(--index_<0)
    index_ = 0;
}
//-------------------------------------------------------------------------------
Query::TokenValue Query::getToken()
{
   wchar_t ch;
   if(!get(ch))
     return TokenValue::END;

   while(ch==' ')
   {
      if(!get(ch))
         return TokenValue::END;

      if((isCyrilic(ch) || ch==L'(' || ch==L'!' ) &&
         (current_==TokenValue::WORD || current_==TokenValue::RP))
      {
        putBack();
        return TokenValue::AND;
      }
   }

   if(isCyrilic(ch))
   {
     WString key;
     bool success= false;
     do
     {
       key += ch;
       success = get(ch);
     }
     while(success && isCyrilic(ch));

     if(success)
       putBack();

     locations_= storage_->get(key);
     return TokenValue::WORD;
   }

   switch(ch)
   {
      case '&':
      {
        if(get(ch) && ch=='&')
          return TokenValue::AND;
        else
          throw Exception("Wrong query!");
      }

      case '|':
      {
        if(get(ch) && ch=='|')
          return TokenValue::OR;
        else
          throw Exception("Wrong query!");
      }

      case '!': return TokenValue::NOT;
      case '(': return TokenValue::LP;
      case ')': return TokenValue::RP;

      default:
        throw Exception("Wrong query!");
   }

   return  TokenValue::NONE;
}
//-------------------------------------------------------------------------------
Locations Query::operationOR(const Locations &left, const Locations &right)
{
  Locations result(left.size()+right.size());
  auto it= user::set_union(left.begin(),left.end(),
                           right.begin(),right.end(),
                           result.begin());
  result.erase(it,result.end());
  return result;
}
//-------------------------------------------------------------------------------
Locations Query::operationAND(const Locations &left, const Locations &right)
{
  Locations result(left.size()+right.size());
  auto it= user::set_intersection(left.begin(),left.end(),
                                  right.begin(),right.end(),
                                  result.begin());
  result.erase(it,result.end());
  return result;
}
//-------------------------------------------------------------------------------
Locations Query::operationNOT(const Locations &other)
{
  Locations result;
  result.reserve(storage_->fileNames().size());
  for(size_t i=0,e=0; i<storage_->fileNames().size(); ++i)
  {
    if(e<other.size())
    {
      if(i==other[e].fileIndex)
      {
        ++e;
        continue;
      }
    }
    result.push_back(Location(i));
  }
  return result;
}
//-------------------------------------------------------------------------------
