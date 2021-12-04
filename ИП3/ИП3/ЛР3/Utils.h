#ifndef UTILS_H
#define UTILS_H
//---------------------------------------------------------------------------------
namespace user
{
//---------------------------------------------------------------------------------
template<class T1,class T2>
struct pair
{
   T1 first;
   T2 second;

   pair(const T1& first,const T2& second)
     :first(first),
      second(second)
   {
   }
};
//---------------------------------------------------------------------------------
template<class T1,class T2>
pair<T1,T2> make_pair(const T1& first,const T2& second)
{
   return pair<T1,T2>(first,second);
}
//---------------------------------------------------------------------------------
template<class It>
void delete_all(It first,It last)
{
  for(; first!=last; ++first)
    delete *first;
}
//---------------------------------------------------------------------------------
template<typename T>
void swap(T& a,T& b)
{
  T temp= a;
  a= b;
  b= temp;
}

template< class ForwardIt, class T >
void fill(ForwardIt first, ForwardIt last, const T& value)
{
    for (; first != last; ++first)
    {
        *first = value;
    }
}

template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last,
              OutputIt d_first)
{
    while (first != last)
    {
       *d_first++ = *first++;
    }
    return d_first;
}
//--------------------------------------------------------------------------------
} //--------------------------------------------------------------------------------
#endif // UTILS_H
