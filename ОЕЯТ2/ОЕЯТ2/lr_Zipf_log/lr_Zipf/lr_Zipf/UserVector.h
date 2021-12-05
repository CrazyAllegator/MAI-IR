#ifndef USERVECTOR_H
#define USERVECTOR_H
//------------------------------------------------------------------------------
#include <limits.h> // UINT_MAX

#include "Utils.h"
// --------------------------------------------------------------------------------
namespace user
{
// --------------------------------------------------------------------------------
template<typename T>
class vector
{
   public:
     typedef T        value_type;
     typedef size_t   size_type;
     typedef size_t   difference_type;

     typedef T&       reference;
     typedef const T& const_reference;

     typedef T*       pointer;
     typedef const T* const_pointer;

     typedef T*       iterator;
     typedef const T* const_iterator;

     typedef T*       reverse_iterator;
     typedef const T* const_reverse_iterator;

     vector();
     explicit vector(size_type size);
              // Fix , const T& value=T()


     // vector( std::initializer_list<T> init)

     vector(const vector& other);
     ~vector();

     vector& operator=(const vector& other);

     vector(vector&& other);
     vector& operator=(vector&& other);

     void assign( size_type count, const T& value );

     template< class InputIt > void assign( InputIt first, InputIt last );

     // reference       at( size_type pos );
     // const_reference at( size_type pos ) const;

     reference operator[](size_type pos);
     const_reference operator[](size_type pos) const;

     reference front();
     const_reference front() const;

     reference back();
     const_reference back() const;

     T* data();
     const T* data() const;

     iterator begin();
     const_iterator begin() const;
     const_iterator cbegin() const;

     iterator end();
     const_iterator end() const;
     const_iterator cend() const;

     void reserve(size_type new_capacity);
     void resize(size_type new_size,const T& value=T());

     size_type capacity() const;

     // void shrink_to_fit();

     void clear();

     iterator insert(iterator where, const T& value );
     iterator insert(iterator where, size_type count, const T& value);

//     iterator insert( const_iterator pos, size_type count, const T& value );
//     template< class InputIt > void insert( iterator pos, InputIt first, InputIt last);
//     template< class InputIt > iterator insert( const_iterator pos, InputIt first, InputIt last );

     iterator erase( iterator where );
//     iterator erase( const_iterator pos );
     iterator erase( iterator first, iterator last );
//     iterator erase( const_iterator first, const_iterator last );

     void push_back(const T& value);
     void pop_back();

     size_type size()const;
     size_type max_size() const;

     bool empty()const;

     void swap(vector& other);

   private:
      size_type calculate_growth(size_type size) const;

   private:
     size_t size_;
     size_t capacity_;
     T* data_;
};


// --------------------------------------------------------------------------------
template<typename T>
vector<T>::vector(const vector<T>& other)
{
   data_ = new T[other.size_];
   user::copy(other.begin(),other.end(),data_);
   capacity_= size_ = other.size_;
}

template<typename T>
vector<T>& vector<T>::operator=(const vector<T>& other)
{
  if(this!=&other)
  {
    if(other.size_<=capacity_)
    {
      size_ = other.size_;
      user::copy(other.begin(),other.end(),data_);
    }
    else
    {
      T* new_data = new T[other.size_];
      user::copy(other.begin(),other.end(),new_data);

      delete[] data_;
      data_= new_data;
      capacity_= size_= other.size_;
    }
  }
  return *this;
}

template<typename T>
void vector<T>::assign(vector<T>::size_type count, const T& value )
{
  if(count<=capacity_)
     size_= count;
  else
  {
     T* new_data = new T[count];
     delete[] data_;
     data_= new_data;
     capacity_= size_ = count;
  }
  user::fill(begin(),end(),value);
}

template<typename T>
template< class InputIt >
void vector<T>::assign( InputIt first, InputIt last )
{
  const size_type count = last-first;
  if(count<=capacity_)
     size_= count;
  else
  {
     T* new_data = new T[count];
     delete[] data_;
     data_= new_data;
     capacity_= size_ = count;
  }
  user::copy(first,last,begin());
}

template<typename T>
void vector<T>::reserve(vector<T>::size_type new_capacity)
{
  if(new_capacity>capacity_)
  {
     T* new_data = new T[new_capacity];
     user::copy(begin(),end(),new_data);

     delete[] data_;
     data_= new_data;
     capacity_= new_capacity;
  }
}

template<typename T>
void vector<T>::resize(vector<T>::size_type new_size, const T& value)
{
  if(new_size<=capacity_)
     size_= new_size;
  else
  {
     const size_type new_capacity= calculate_growth(new_size);
     T* new_data = new T[new_capacity];
     user::copy(begin(),end(),new_data);
     user::fill(new_data+size_, new_data+new_size, value);

     delete[] data_;
     data_= new_data;
     size_ = new_size;
     capacity_= new_capacity;
  }
}

template<typename T>
void vector<T>::push_back(const T& value)
{
  if(size_<capacity_)
     data_[size_++]= value;
  else
  {
    const size_type new_capacity= calculate_growth(size_+1);
    T* new_data = new T[new_capacity];
    user::copy(begin(),end(),new_data);
    new_data[size_++]= value;

    delete[] data_;
    data_= new_data;
    capacity_= new_capacity;
  }
}

template<typename T>
typename vector<T>::iterator
   vector<T>::insert(iterator where, size_type count, const T& value)
{
   if(count==0)
     return where;

   const size_type unused_capacity= capacity_-size_;
   if(count<=unused_capacity)
   {
     size_ += count;
     user::shift_right(where,end(),count);
     user::fill(where,where+count,value);
     return where;
   }
   else
   {
     const size_type new_size= size_+count;
     const size_type new_capacity= calculate_growth(new_size);
     const size_type offset= where-begin();

     T* new_data = new T[new_capacity];
     T* left =  new_data+offset;
     T* right = left+count;

     user::copy(begin(),where,new_data);
     user::copy(where,end(),right);
     user::fill(left,right,value);

     delete[] data_;
     data_ = new_data;
     size_ = new_size;
     capacity_ = new_capacity;
     return left;
   }
}


/* private  */
template<typename T>
typename vector<T>::size_type
   vector<T>::calculate_growth(typename vector<T>::size_type new_size) const
{
  if(capacity_ > max_size()-capacity_/2)
  {
    return new_size;
  }

  const size_type geometric = capacity_ + capacity_ / 2;
  if (geometric < new_size)
  {
    return new_size;	// geometric growth would be insufficient
  }

  return geometric;	// geometric growth is sufficient
}





template<typename T>
vector<T>::vector(vector<T>&& other)
   :vector()
{
	this->swap(other); 
}

template<typename T>
vector<T>& vector<T>::operator=(vector<T>&& other)
{
   if(this != &other)
      this->swap(other);
   return *this;
}

template<typename T>
typename vector<T>::iterator
   vector<T>::insert(vector<T>::iterator where, const T& value)
{
   return insert(where,1,value);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(iterator first, iterator last)
{
  const size_type count= last-first;
  user::shift_left(first,end(),count);
  size_ -= count;

  return first;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector<T>::iterator where)
{
  return erase(where,where+1);
}

template<typename T>
void vector<T>::swap(vector<T>& other)
{
  user::swap(capacity_,other.capacity_);
  user::swap(size_,other.size_);
  user::swap(data_,other.data_);
}

template<typename T>
vector<T>:: vector()
  :size_(0),
   capacity_(0),
   data_(0)
{
}

template<typename T>
vector<T>::vector(size_type size)
  :size_(size),
   capacity_(size),
   data_(new T[capacity_])
{

}

template<typename T>
vector<T>::~vector()
{
  delete[] data_;
}

template<typename T>
typename vector<T>::reference
   vector<T>::operator[](vector<T>::size_type pos)
{
  return data_[pos];
}

template<typename T>
typename vector<T>::const_reference
   vector<T>::operator[](size_type pos) const
{
  return data_[pos];
}

template<typename T>
typename vector<T>::reference vector<T>::front()
{
  return data_[0];
}

template<typename T>
typename vector<T>::const_reference vector<T>::front() const
{
  return data_[0];
}

template<typename T>
typename vector<T>::reference vector<T>::back()
{
  return data_[size_-1];
}

template<typename T>
typename vector<T>::const_reference vector<T>::back() const
{
  return data_[size_-1];
}

template<typename T>
T* vector<T>::data()
{
  return data_;
}

template<typename T>
const T* vector<T>::data() const
{
  return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::begin()
{
  return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const
{
  return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::cbegin() const
{
  return data_;
}

template<typename T>
typename vector<T>::iterator vector<T>::end()
{
  return data_+size_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const
{
  return data_+size_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::cend() const
{
 return data_+size_;
}

template<typename T>
void vector<T>::pop_back()
{
  --size_;
}

template<typename T>
typename vector<T>::size_type vector<T>::max_size() const
{
  return UINT_MAX;
}

template<typename T>
void vector<T>::clear()
{
  size_= 0;
}

template<typename T>
size_t vector<T>::size() const
{
  return size_;
}

template<typename T>
bool vector<T>::empty() const
{
  return size_==0;
}

template<typename T>
typename vector<T>::size_type vector<T>::capacity() const
{
  return capacity_;
}
//------------------------------------------------------------------------------
}; // end user namespace
//------------------------------------------------------------------------------
#endif // USERVECTOR_H
