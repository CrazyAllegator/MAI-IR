#ifndef STORAGE_H
#define STORAGE_H
//----------------------------------------------------------------------------------
#include "Global.h"
#include "FS.h"
#include "Tokenizing.h"
#include "UserHashTable.h"
//----------------------------------------------------------------------------------
class Storage
{
  public:
    Storage(size_t bucket_size= 400'000);

    Storage(const Storage& other)=delete;
    Storage& operator=(const Storage& other)=delete;

    bool index(size_t maxFileCount=0);

    const WStrings& fileNames()const;
    const WStrings& snippets()const;

    void load(const WString& fileName);
    void save(const WString& fileName);

    Locations get(const WString& key);

    void printTable()const;
    void printFileNames()const;
    void printSnippets() const;

    void printReport() const;

    unsigned long totalTokenSize() const;
    void setTotalTokenSize(unsigned long totalTokenSize);

    size_t size()const;

    const WString& dir()const;
    void setDir(const WString& dir);

private:
    WString dir_;

    WStrings fileNames_;
    WStrings snippets_;

    unsigned long totalTokenCount_ = 0;
    unsigned long totalDuration_ = 0;
    unsigned long totalTokenLength_= 0;
    unsigned long totalBytes_ = 0;

    user::hash_table<WString, Locations> table_;
};
//----------------------------------------------------------------------------------
#endif // STORAGE_H
