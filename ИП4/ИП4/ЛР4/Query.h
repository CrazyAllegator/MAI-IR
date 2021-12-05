#ifndef QUERY_H
#define QUERY_H
//------------------------------------------------------------
#include "Global.h"
#include "Exception.h"
#include "Storage.h"
//------------------------------------------------------------
class Query
{
public:
     explicit Query(Storage *storage, const WString& queryStr=WString());
     Locations prepare();

     bool isValid();

     void setQueryString(const WString& queryStr);

     WString queryString()const;

private:
     bool get(wchar_t &ch);
     void putBack();

     enum class TokenValue
     {
       NONE,
       OR, AND, NOT,
       WORD,
       LP, RP,
       END
     };

     TokenValue getToken();

     Locations level_1(bool needToken); // OR
     Locations level_2(bool needToken); // AND
     Locations level_3(bool needToken); // () NOT

     Locations operationOR(const  Locations& left,const  Locations& right);
     Locations operationAND(const  Locations& left, const  Locations& right);
     Locations operationNOT(const  Locations& other);

private:
     Storage* storage_;

     WString queryStr_;
     Locations locations_;
     size_t index_;
     TokenValue current_;
};
//------------------------------------------------------------
#endif // QUERY_H
