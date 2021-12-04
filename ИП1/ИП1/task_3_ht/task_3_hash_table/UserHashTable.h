#ifndef HASH_TABLE_H
#define HASH_TABLE_H
//-----------------------------------------------------------------------
#include "Utils.h"
#include "UserString.h"
#include "UserVector.h"
#include "UserList.h"

#include "murmur_hash2.h"
//-----------------------------------------------------------------------
namespace user
{
//-----------------------------------------------------------------------
template <typename T> struct hash;
//-----------------------------------------------------------------------
template<>
struct hash<user::wstring>
{
    public:
       size_t operator()(const wstring& ws) const
       {
         return murmur_hash2( reinterpret_cast<char*>(const_cast<wchar_t*>(ws.data())),
                              ws.size()*sizeof(wstring::value_type));
       }
};
//-----------------------------------------------------------------------
template<class Key,
         class T,
         class Hash= user::hash<Key> >
class hash_table
{
      typedef user::list<user::pair<Key,T>> entry_type;

  public:
      hash_table(size_t bucket_size)
        :size_(0)
      {
         buckets_.resize(bucket_size);
         user::fill(buckets_.begin(),buckets_.end(),nullptr);
      }

      ~hash_table()
      {
        user::delete_all(buckets_.begin(),buckets_.end());
      }

      void clear()
      {
        user::delete_all(buckets_.begin(),buckets_.end());
        user::fill(buckets_.begin(),buckets_.end(),nullptr);
        size_= 0;
      }

      void set_bucket_size(size_t bucket_size)
      {
        buckets_.resize(bucket_size);
      }

      T& operator[](const Key& key)
      {
        const size_t hash_code=  hash_(key) % buckets_.size();
        if(buckets_[hash_code])
        {
          for(auto it=buckets_[hash_code]->begin(); it!=buckets_[hash_code]->end(); ++it)
          {
            if(it->first==key)
               return it->second;
          }
          //printf("Collision: %d\n",buckets_[hash_code]->size()+1);
        }
        else
        {
          buckets_[hash_code]= new entry_type;
        }
        ++size_;
        buckets_[hash_code]->push_back(user::make_pair(key,T()));
        return buckets_[hash_code]->back().second;
      }

      hash_table(const hash_table&)=delete;
      hash_table& operator=(const hash_table&)=delete;

      const user::vector<entry_type*>& buckets()const
      {
        return buckets_;
      }

      size_t size()const
      {
        return size_;
      }

  private:
      user::vector<entry_type*> buckets_;
      Hash hash_;
      size_t size_;
};
//-----------------------------------------------------------------------
}
//-----------------------------------------------------------------------
#endif // HASH_TABLE_H
