#ifndef USERSTRING_H
#define USERSTRING_H
//---------------------------------------------------------------
#include "Utils.h"

#include <string.h>
#include <limits.h>
// --------------------------------------------------------------------------------
namespace user
{
//---------------------------------------------------------------
template<typename CharT>
class basic_string;
//---------------------------------------------------------------
template<typename CharT>
basic_string<CharT> concat(const basic_string<CharT>& str,const CharT* cstr);

template<typename CharT>
basic_string<CharT> concat(const CharT* cstr,const basic_string<CharT>& str);
//---------------------------------------------------------------
template<typename CharT>
class basic_string
{
   public:
     typedef CharT value_type;
     typedef size_t size_type;

     static const size_t npos = size_t(-1);

     basic_string();
     explicit basic_string(size_t size, CharT fill_char=0);
     basic_string(const basic_string& str);
     basic_string(const CharT* cStr);

     ~basic_string();

     basic_string(basic_string&& str);
     basic_string& operator=(basic_string&& str);

     basic_string& operator=(const basic_string& str);
     basic_string& operator=(const CharT* cstr);

     basic_string operator+(const basic_string& str)const;

     friend basic_string concat<CharT>(const basic_string<CharT>& str,const CharT* cstr);
     friend basic_string concat<CharT>(const CharT* cstr,const basic_string<CharT>& str);

     friend basic_string operator+(const basic_string& str,const CharT* cstr){ return concat(str,cstr); }
     friend basic_string operator+(const CharT* cstr,const basic_string& str){ return concat(cstr,str); }

     basic_string& operator+=(const basic_string& str);
     basic_string& operator+=(const CharT* cstr);
     basic_string& operator+=(CharT c);

     bool operator==(const basic_string& str)const;
     bool operator!=(const basic_string& str)const;

     bool operator<(const basic_string& str)const;
     bool operator>(const basic_string& str)const;

     bool operator==(const CharT* cStr)const;
     bool operator!=(const CharT* cStr)const;
     bool operator<(const CharT* cStr)const;
     bool operator>(const CharT* cStr)const;

     typedef CharT* iterator;
     typedef const CharT* const_iterator;

     iterator begin();
     iterator end();

     const_iterator begin()const;
     const_iterator end()const;

     CharT& operator[](size_t index);
     const CharT& operator[](size_t index)const;

     const CharT* c_str()const;
     const CharT* data()const;
     CharT* data();

     size_t max_size() const;

     void reserve(size_t newCapacity);
     void resize(size_t newSize);
     void clear();

     template< class InputIt>
     iterator insert(iterator where, InputIt first,InputIt last);

     iterator erase(iterator first, iterator last);

     basic_string substr(size_t pos, size_t len) const;
     size_t find(const basic_string &str, size_type pos = 0)const;

     size_t size()const;
     inline size_t length()const{ return size(); }

     size_t capacity()const;
     bool empty()const;
     void swap(basic_string& other);

   private:
     size_t calculate_growth(size_t new_size) const;
     basic_string &append(const CharT *cstr, size_t c_size);

   private:
     size_t size_;
     size_t capacity_;
     CharT*  data_;
};
//---------------------------------------------------------------
template<typename CharT>
template<typename InputIt>
typename basic_string<CharT>::iterator
   basic_string<CharT>::insert(iterator where, InputIt first, InputIt last)
{
   size_t count = last - first;
   if(count==0)
     return where;

   const size_type unused_capacity= capacity_-size_;
   if(count<=unused_capacity)
   {
     size_ += count;
     user::shift_right(where,end(),count);
     user::copy(first,last,where);
     data_[size_]= 0;
     return where;
   }
   else
   {
     const size_type new_size= size_+count;
     const size_type new_capacity= calculate_growth(new_size);
     const size_type offset= where-begin();

     CharT* new_data = new CharT[new_capacity+1];
     CharT* left =  new_data+offset;
     CharT* right = left+count;

     user::copy(begin(),where,new_data);
     user::copy(where,end(),right);
     user::copy(first,last,left);

     delete[] data_;
     data_ = new_data;
     size_ = new_size;
     capacity_ = new_capacity;
     data_[size_]= 0;

     return left;
   }
}
//---------------------------------------------------------------
template<typename CharT>
typename basic_string<CharT>::iterator basic_string<CharT>::erase(iterator first, iterator last)
{
  const size_type count= last-first;
  user::shift_left(first,end(),count);
  size_ -= count;

  return first;
}
//---------------------------------------------------------------
template<typename CharT>
inline size_t c_str_size(const CharT* cstr)
{
  size_t i=0;
  while(cstr[i]!=0)
    ++i;
  return i;
}

inline int c_str_cmp(const char* cstr1,const char* cstr2)
{
  return strcmp(cstr1,cstr2);
}

inline int c_str_cmp(const wchar_t* wcstr1,const wchar_t* wcstr2)
{
  return  wcscmp(wcstr1,wcstr2);
}
//---------------------------------------------------------------
template<typename CharT>
basic_string<CharT>::basic_string()
  :size_(0),
   capacity_(0),
   data_(new CharT[1])
{
  data_[0] = 0;
}

template<typename CharT>
basic_string<CharT>::basic_string(size_t size, CharT fill_char)
  :size_(size),
   capacity_(size),
   data_(new CharT[size+1])
{
  user::fill(begin(),end(),fill_char);
  data_[size_]= 0;
}

template<typename CharT>
basic_string<CharT>::basic_string(const basic_string<CharT> &str)
{
  capacity_= size_ = str.size_;
  data_ = new CharT[size_+1];
  user::copy(str.begin(),str.end(),begin());
  data_[size_]= 0;
}

template<typename CharT>
basic_string<CharT>::basic_string(const CharT *cstr)
{
  capacity_= size_= c_str_size(cstr);
  data_ = new CharT[size_+1];
  user::copy(cstr,cstr+size_,data_);
  data_[size_]= 0;
}

template<typename CharT>
basic_string<CharT>::~basic_string()
{
  delete [] data_;
}


template<typename CharT>
basic_string<CharT>::basic_string(basic_string &&str)
  :basic_string()
{
	this->swap(str);
}

template<typename CharT>
basic_string<CharT> &basic_string<CharT>::operator=(basic_string &&str)
{
  if(this!=&str)
	  this->swap(str);

  return *this;
}

template<typename CharT>
basic_string<CharT> &basic_string<CharT>::operator=(const basic_string &other)
{
  if(this==&other)
    return *this;

  if(other.size_<=capacity_)
  {
     size_ = other.size_;
  }
  else
  {
    CharT* new_data = new CharT[other.size_+1];
    capacity_= size_= other.size_;
    delete[] data_;
    data_= new_data;
  }

  user::copy(other.data_,other.data_+other.size_,data_);
  data_[size_]= 0;

  return *this;
}

template<typename CharT>
basic_string<CharT>& basic_string<CharT>::operator=(const CharT *cstr)
{
  if(data_==cstr)
    return *this;

  const size_t new_size= size_= c_str_size(cstr);
  if(new_size<=capacity_)
  {
     size_= new_size;
  }
  else
  {
    CharT* new_data= new CharT[new_size+1];
    capacity_= size_= new_size;
    delete[] data_;
    data_= new_data;
  }

  user::copy(cstr,cstr+size_,data_);
  data_[size_]= 0;

  return *this;
}

template<typename CharT>
basic_string<CharT> basic_string<CharT>::operator+(const basic_string &other) const
{
 basic_string<CharT> result(size_+other.size_);

 user::copy(begin(),end(),result.begin());
 user::copy(other.begin(),other.end(),result.data_+size_);
 result.data_[result.size_]= 0;

 return result;
}

template<typename CharT>
basic_string<CharT> concat(const basic_string<CharT>& str,const CharT* cstr)
{
    const size_t csize = c_str_size(cstr);
    basic_string<CharT> result(str.size_+csize);

    user::copy(str.begin(),str.end(),result.begin());
    user::copy(cstr,cstr+csize,result.data_+str.size_);
    result.data_[result.size_]= 0;

    return result;
}

template<typename CharT>
basic_string<CharT> concat(const CharT *cstr, const basic_string<CharT> &str)
{
   const size_t csize = c_str_size(cstr);
   basic_string<CharT> result(csize+str.size_);

   user::copy(cstr,cstr+csize, result.begin());
   user::copy(str.begin(),str.end(), result.data_+csize);
   result.data_[result.size_]= 0;

   return result;
}


template<typename CharT>
basic_string<CharT>& basic_string<CharT>::append(const CharT *cstr, size_t cSize)
{
  const size_t unusedCapacity = capacity_-size_;
  if(cSize<=unusedCapacity)
  {
    user::copy(cstr,cstr+cSize,data_+size_);
    size_ += cSize;
  }
  else
  {
    const size_t new_size= size_+cSize;
    const size_t new_capacity= calculate_growth(new_size);
    CharT* new_data = new CharT[new_capacity+1];

    user::copy(begin(),end(),new_data);
    user::copy(cstr,cstr+cSize,new_data+size_);

    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
    size_=  new_size;
    data_[size_]= 0;
  }
  return *this;
}

template<typename CharT>
basic_string<CharT> &basic_string<CharT>::operator+=(const basic_string &str)
{
  return append(str.data_,str.size_);
}

template<typename CharT>
basic_string<CharT> &basic_string<CharT>::operator+=(const CharT *cstr)
{
  return append(cstr,c_str_size(cstr));
}

template<typename CharT>
basic_string<CharT> &basic_string<CharT>::operator+=(CharT c)
{
  return append(&c,1);
}

template<typename CharT>
bool basic_string<CharT>::operator==(const basic_string &str) const
{
  if(size_!=str.size())
    return false;

  for(size_t i=0; i<size_; ++i)
    if(data_[i]!=str.data_[i])
      return false;

  return true;
}

template<typename CharT>
bool basic_string<CharT>::operator==(const CharT *c_str) const
{
  return c_str_cmp(data_,c_str)==0;
}

template<typename CharT>
bool basic_string<CharT>::operator!=(const basic_string &str) const
{
  return !(*this==str);
}

template<typename CharT>
bool basic_string<CharT>::operator!=(const CharT *cstr) const
{
  return c_str_cmp(data_,cstr)!=0;
}


template<typename CharT>
bool basic_string<CharT>::operator<(const basic_string &str) const
{
  return c_str_cmp(data_,str.data_)<0;
}

template<typename CharT>
bool basic_string<CharT>::operator<(const CharT *cstr) const
{
  return c_str_cmp(data_,cstr)<0;
}

template<typename CharT>
bool basic_string<CharT>::operator>(const basic_string &str) const
{
  return c_str_cmp(data_,str.data_)>0;
}

template<typename CharT>
bool basic_string<CharT>::operator>(const CharT *cstr) const
{
  return c_str_cmp(data_,cstr)>0;
}

template<typename CharT>
typename basic_string<CharT>::iterator basic_string<CharT>::begin()
{
  return data_;
}

template<typename CharT>
typename basic_string<CharT>::const_iterator basic_string<CharT>::begin() const
{
  return data_;
}

template<typename CharT>
typename basic_string<CharT>::iterator basic_string<CharT>::end()
{
  return data_+size_;
}

template<typename CharT>
typename basic_string<CharT>::const_iterator basic_string<CharT>::end() const
{
  return data_+size_;
}

template<typename CharT>
CharT &basic_string<CharT>::operator[](size_t index)
{
  return data_[index];
}

template<typename CharT>
const CharT &basic_string<CharT>::operator[](size_t index) const
{
  return data_[index];
}

template<typename CharT>
const CharT *basic_string<CharT>::c_str() const
{
  return data_;
}

template<typename CharT>
const CharT *basic_string<CharT>::data() const
{
  return data_;
}

template<typename CharT>
CharT *basic_string<CharT>::data()
{
  return data_;
}

template<typename CharT>
size_t basic_string<CharT>::max_size() const
{
  return UINT_MAX;
}

template<typename CharT>
void basic_string<CharT>::reserve(size_t new_capacity)
{
  if(new_capacity>capacity_)
  {
     CharT* new_data = new CharT[new_capacity+1];
	 user::copy(begin(),end(),new_data);

     delete[] data_;
     data_= new_data;
     capacity_= new_capacity;
  }
}

template<typename CharT>
void basic_string<CharT>::resize(size_t new_size)
{
  if(new_size<=capacity_)
  {
     size_= new_size;
     data_[size_] = 0;
  }
  else
  {
     const size_t new_capacity= calculate_growth(new_size);
     CharT* new_data = new CharT[new_capacity+1];
	 user::copy(begin(),end(),new_data);

     delete[] data_;
     data_= new_data;
     size_ = new_size;
     capacity_= new_capacity;
  }
}

template<typename CharT>
void basic_string<CharT>::clear()
{
  size_= 0;
  data_[0]='\0';
}

template<typename CharT>
size_t basic_string<CharT>::find(const basic_string<CharT> &str,
                                 basic_string<CharT>::size_type pos)const
{
  if(size_<str.size())
    return npos;

  for(size_t i=pos; i<size_-str.size(); ++i)
  {
    bool matched= true;
    for(size_t g=0; g<str.size(); ++g)
    {
      if(data_[i+g]!=str[g])
      {
        matched= false;
        break;
      }
    }
    if(matched)
      return i;
  }
  return npos;
}

template<typename CharT>
basic_string<CharT> basic_string<CharT>::substr(size_t pos, size_t len) const
{
  basic_string<CharT> result(len,0);
  len=  (pos+len<=size_) ? len : size_-pos;
  user::copy(data_+pos,data_+pos+len,result.begin());
  return result;
}

template<typename CharT>
size_t basic_string<CharT>::calculate_growth(size_t new_size) const
{
  if(capacity_ > max_size()-capacity_/2)
  {
    return new_size;
  }

  const size_t geometric = capacity_ + capacity_ / 2;
  if (geometric < new_size)
  {
    return new_size;
  }

  return geometric;
}

template<typename CharT>
size_t basic_string<CharT>::size() const
{
  return size_;
}

template<typename CharT>
size_t basic_string<CharT>::capacity() const
{
  return capacity_;
}

template<typename CharT>
bool basic_string<CharT>::empty() const
{
  return size_==0;
}

template<typename CharT>
void basic_string<CharT>::swap(basic_string &other)
{
  user::swap(data_,other.data_);
  user::swap(size_,other.size_);
  user::swap(capacity_,other.capacity_);
}
//---------------------------------------------------------------
typedef basic_string<char>    string;
typedef basic_string<wchar_t> wstring;
//---------------------------------------------------------------
}
//---------------------------------------------------------------
#endif // USERSTRING_H
