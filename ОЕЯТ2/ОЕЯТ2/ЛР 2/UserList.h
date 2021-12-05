#ifndef USERLIST_H
#define USERLIST_H
//-----------------------------------------------------
#include "Utils.h"
//-----------------------------------------------------
namespace user
{
//-----------------------------------------------------
template<typename T>
struct node
{
  T value;
  node<T>* prev;
  node<T>* next;

  node(const T& value,
       node<T>* prev=nullptr,
       node<T>* next=nullptr)
  :value(value),
    prev(prev),
    next(next)
  {
  }
};
//-----------------------------------------------------
template<typename T>
  class list_iterator
{

  public:
    list_iterator(node<T>* ptr=nullptr):ptr_(ptr)
    {
    }

    T& operator*() const
    {
      return ptr_->value;
    }

    T* operator->() const
    {
      return &(ptr_->value);
    }

    list_iterator& operator++()
    {
      ptr_= ptr_->next ? ptr_->next : nullptr;
      return (*this);
    }

    list_iterator operator++(int)
    {
      list_iterator tmp = *this;
      ++*this;
      return tmp;
    }

    list_iterator& operator--()
    {
      ptr_= ptr_->prev;
      return (*this);
    }

    list_iterator operator--(int)
    {
     list_iterator tmp = *this;
     --*this;
     return tmp;
    }

    bool operator==(const list_iterator& right) const
    {
      return ptr_ == right.ptr_;
    }

    bool operator!=(const list_iterator& right) const
    {
      return ptr_ != right.ptr_;
    }

  private:
    node<T>* ptr_;
};
//-----------------------------------------------------
template<typename T>
class list
{
  public:
     typedef list_iterator<T> iterator;

     list();

     list(const list& other);
     list& operator=(const list& other);

     list(list&& other);
     list& operator=(list&& other);

     ~list();
     void clear();

     void push_back(const T& el);
     void push_front(const T& el);

     iterator begin();
     iterator end();

     iterator begin()const;
     iterator end()const;

     bool empty()const;

     size_t size()const;

     T& front();
     T& back();

  private:
     node<T>* head_;
     node<T>* tail_;
};

template<typename T>
list<T>::list()
  :head_(nullptr),
   tail_(nullptr)
{

}

template<typename T>
list<T>::list(const list<T> &other)
  :list()
{
  for(iterator it=other.begin(); it!=other.end(); ++it)
    push_back(*it);
}

template<typename T>
list<T> &list<T>::operator=(const list<T> &other)
{
  clear();
  for(iterator it=other.begin(); it!=other.end(); ++it)
    this->push_back(*it);
  return *this;
}


template<typename T>
list<T>::list(list<T> && other)
 :list()
{
  user::swap(head_,other.head_);
  user::swap(tail_,other.tail_);
}

template<typename T>
list<T> &list<T>::operator=(list<T> &&other)
{
  user::swap(head_,other.head_);
  user::swap(tail_,other.tail_);
  return *this;
}

template<typename T>
list<T>::~list()
{
  clear();
}

template<typename T>
void list<T>::clear()
{
  while(head_)
  {
    node<T>* current = head_;
    head_ = head_->next;
    delete current;
  }
  head_ = tail_ = nullptr;
}

template<typename T>
bool list<T>::empty()const
{
  return !head_ && !tail_;
}

template<typename T>
size_t list<T>::size() const
{
  size_t size_=0;
  for(iterator it=begin(); it!=end(); ++it)
    ++size_;
  return size_;
}

template<typename T>
T& list<T>::front()
{
  return head_->value;
}

template<typename T>
T& list<T>::back()
{
  return tail_->value;
}

template<typename T>
void list<T>::push_back(const T& value)
{
  node<T>* new_node= new node<T>(value);
  if(empty())
  {
    head_= tail_= new_node;
  }
  else
  {
    tail_->next= new_node;
    new_node->prev= tail_;
    tail_= new_node;
  }
}

template<typename T>
void list<T>::push_front(const T& value)
{
  node<T>* new_node= new node<T>(value);

  if(empty())
  {
    head_ = tail_= new_node;
  }
  else
  {
    head_->prev= new_node;
    new_node->next= head_;
    head_= new_node;
  }
}


template<typename T>
typename list<T>::iterator list<T>::begin()
{
  return iterator(head_);
}

template<typename T>
typename list<T>::iterator list<T>::end()
{
  return iterator(nullptr);
}

template<typename T>
typename list<T>::iterator list<T>::begin() const
{
 return iterator(head_);
}

template<typename T>
typename list<T>::iterator list<T>::end() const
{
  return iterator(nullptr);
}
//-----------------------------------------------------
}
//-----------------------------------------------------
#endif // SPECIALLIST_H
