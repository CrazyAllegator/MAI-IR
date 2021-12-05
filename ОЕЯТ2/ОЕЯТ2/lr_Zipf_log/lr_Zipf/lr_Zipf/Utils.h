#ifndef UTILS_H
#define UTILS_H
//---------------------------------------------------------------------------------
/*
 * Алгоритмы позаимствованные из STL и модифицированные под
 * данную задачу. В частности используются в самописных контейнерах.
 */
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
  T temp= a; // std::move(a)  > C++11
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
template<class ForwardIt1, class ForwardIt2>
constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                            ForwardIt2 s_first, ForwardIt2 s_last)
{
    for (; ; ++first)
    {
        ForwardIt1 it = first;
        for (ForwardIt2 s_it = s_first; ; ++it, ++s_it)
        {
          if(s_it == s_last)
              return first;
          if(it == last)
              return last;
          if(!(*it == *s_it))
             break;
        }
    }
}
//--------------------------------------------------------------------------------
template<typename T>
void shift_right(T* first, T* last, size_t count)
{
  --last;
  T* middle = last-count;
  for( ; middle>=first; --middle,--last)
    *last = *middle;
}


template<typename T>
void shift_left(T* first, T* last, size_t count)
{
  T* middle = first+count;
  for( ; middle!=last ; ++first,++middle)
     *first = *middle;
}


template<typename T>
const T& min_(const T& a, const T& b)
{
    return (b < a) ? b : a;
}


template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate p)
{
    for(; first != last; ++first)
    {
        if (p(*first))
        {
           return first;
        }
    }
    return last;
}


template<class ForwardIt, class T>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value)
{
    ForwardIt it;
    size_t count, step;
    count = last-first;

    while (count > 0)
    {
        it = first;
        step = count / 2;
        it+= step;
        if (*it < value)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }
    return first;
}


template<class ForwardIt, class T, class Compare>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
	ForwardIt it;
	size_t count, step;
	count = last - first;

	while (count > 0) 
	{
		it = first;
		step = count / 2;
		it += step;
		if (comp(*it, value)) 
		{
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first;
}

// AND
template<class InputIt1, class InputIt2, class OutputIt>
OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                          InputIt2 first2, InputIt2 last2,
                          OutputIt d_first)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 < *first2)
        {
            ++first1;
        }
        else
        {
            if (!(*first2 < *first1))
            {
                *d_first++ = *first1++;
            }
            ++first2;
        }
    }
    return d_first;
}

// OR
template<class InputIt1, class InputIt2, class OutputIt>
OutputIt set_union(InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, InputIt2 last2,
                   OutputIt d_first)
{
    for (; first1 != last1; ++d_first)
    {
        if (first2 == last2)
            return user::copy(first1, last1, d_first);
        if (*first2 < *first1)
        {
            *d_first = *first2++;
        } else
        {
            *d_first = *first1;
            if (!(*first1 < *first2))
                ++first2;
            ++first1;
        }
    }
    return user::copy(first2, last2, d_first);
}
//--------------------------------------------------------------------------------
} // end user namespace
//--------------------------------------------------------------------------------
#endif // UTILS_H
