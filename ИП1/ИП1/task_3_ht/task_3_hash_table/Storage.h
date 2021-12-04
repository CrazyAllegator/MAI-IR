#ifndef STORAGE_H
#define STORAGE_H

#include "Global.h"
#include "FS.h"
#include "Tokenizing.h"
#include "UserHashTable.h"
//----------------------------------------------------------------------------------
typedef List<Location> Locations;
//----------------------------------------------------------------------------------
/*
 * Класс хранилища где будут хранится индексы
 * токены и их положение в файлах-статей
*/
//----------------------------------------------------------------------------------
class Storage
{
  public:
    Storage(size_t bucket_size= 400'000);

    Storage(const Storage& other)=delete;
    Storage& operator=(const Storage& other)=delete;

    bool index(const WString& dir, size_t maxFileCount);

    const WStrings& fileNames()const;
    const WStrings& snippets()const;

    void load(const WString& fileName);
    void save(const WString& fileName);

    void printTable()const;
    void printFileNames()const;
    void printSnippets() const;

  private:
    WStrings fileNames_;
    WStrings snippets_;
    user::hash_table<WString, Locations> table_;
};
//----------------------------------------------------------------------------------
#endif // STORAGE_H
