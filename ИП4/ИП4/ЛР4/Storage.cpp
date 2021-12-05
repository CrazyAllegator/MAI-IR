#include "Storage.h"
#include "Encoding.h"
#include "Utils.h"
#include "Serialization.h"

#include <stdio.h> 
//-------------------------------------------------------------------------------------------
Storage::Storage(size_t bucket_size)
  :table_(bucket_size)
{
  fileNames_.reserve(40000);
  snippets_.reserve(40000);
}
//------------------------------------------------------------------
bool Storage::index(size_t maxFileCount)
{
  fileNames_.clear();
  snippets_.clear();
  table_.clear();

  printf("Поиск файлов ...\n");
  const size_t count= entry(fileNames_,dir_,L"*.txt",AllFiles);

  if(maxFileCount)
    fileNames_.resize(maxFileCount);

  unsigned long startTime=  GetTickCount();

  printf("Обработка файлов ...\n");
  for(size_t f=0; f<fileNames_.size(); ++f)
  {
    size_t endOfSnippet= 0;

    const String printedFileName = wStringToBytes(fileNames_[f],CP_UTF8);
    printf("%d. \"%s\"\n",f+1,printedFileName.c_str());

    const WString path=  join(dir_,fileNames_[f]);
    const String  bytes= readAllText(path);
    totalBytes_ += bytes.size();

    const WString text=  bytesToWString(bytes,CP_UTF8);
    Tokens tokens= tokenize(text);
    totalTokenCount_ += tokens.size();

    for(size_t t=0; t<tokens.size(); ++t)
    {
      totalTokenLength_+= tokens[t].token.length();
      tokens[t].location.fileIndex= f;

      Locations& v = table_[tokens[t].token];
      auto it = user::lower_bound(v.begin(),v.end(),tokens[t].location);
      if(it==v.end() || (*it)!=tokens[t].location)
        v.insert(it,tokens[t].location);

      if(endOfSnippet==0 && tokens[t].location.pos>=200)
        endOfSnippet= tokens[t].location.pos + tokens[t].token.size();
    }
    snippets_.push_back(text.substr(0,endOfSnippet));
  }

  totalDuration_ = GetTickCount() - startTime;
  return count!=0;
}
//------------------------------------------------------------------
void Storage::load(const WString &fileName)
{
  printf("Загрузка файла ...\n");
  fileNames_.clear();
  snippets_.clear();
  table_.clear();

  File file;
  file.open(fileName.c_str(), GENERIC_READ,  OPEN_EXISTING);

  deserialize(fileNames_, file);
  deserialize(snippets_, file);

  size_t token_count= 0;
  deserialize(token_count,file);

  for(size_t i=0; i<token_count; ++i)
  {
    WString token;
    deserialize(token, file);

    Locations locations;
    deserialize(locations,file);

    table_[token]= locations;
  }
}
//------------------------------------------------------------------
void Storage::save(const WString &fileName)
{
  printf("Сохранение результата ...\n");

  File file;
  file.open(fileName.c_str(), GENERIC_WRITE, CREATE_ALWAYS);

  serialize(fileNames_, file);
  serialize(snippets_, file);

  serialize(table_.size(),file);
  const auto& buckets = table_.buckets();
  for(const auto& bucket:buckets)
  {
    if(bucket)
    {
      for(const auto& pair: *bucket)
      {
        serialize(pair.first,file);
        serialize(pair.second,file);
      }
    }
  }
}
//-------------------------------------------------------------------------------------------
Locations Storage::get(const WString &key)
{
  const Locations* location = table_.get(key);
  if(location)
      return *location;
  return Locations();
}
//-------------------------------------------------------------------------------------------
void Storage::printFileNames() const
{
  printf("Файлы:\n");
  for(size_t i=0; i<fileNames_.size(); ++i)
  {
    const String fileName = wStringToBytes(fileNames_[i],CP_UTF8);
    printf("%d. %s\n",i+1,fileName.c_str());
  }
}
//-------------------------------------------------------------------------------------------
void Storage::printSnippets() const
{
  printf("Cниппеты:\n");
  for(size_t i=0; i<snippets_.size(); ++i)
  {
    const String snippet = wStringToBytes(snippets_[i],CP_UTF8);
    printf("%d. <<%s>>\n\n",i+1,snippet.c_str());
  }
}
//-------------------------------------------------------------------------------------------
void Storage::printTable() const
{
  printf("Вывод результата ...\n");
  const auto& buckets = table_.buckets();
  int i=1;
  for(const auto& bucket:buckets)
  {
    if(bucket)
    {
      for(const auto& pair: *bucket)
      {
        const String token = wStringToBytes(pair.first,CP_UTF8);
        printf("%d. %s\n",i++,token.c_str());

        const auto& locations = pair.second;
        for(const auto& location:locations)
        {
           const WString& fileNameW = fileNames_[location.fileIndex];
           const String fileName = wStringToBytes(fileNameW,CP_UTF8);
           printf("\tnumber:%d pos:%d file:\"%s\"\n",
                  location.tokenNumber,
                  location.pos,
                  fileName.c_str());
        }
      }
    }
  }
}
//------------------------------------------------------------------
const WStrings& Storage::fileNames()const
{
  return fileNames_;
}
//------------------------------------------------------------------
const WStrings& Storage::snippets()const
{
  return snippets_;
}
//-------------------------------------------------------------------------------------------
void Storage::printReport() const
{
   printf("============== REPORT ==============\n");
   printf("Duration: %lu ms\n",  totalDuration_ );
   printf("Bytes: %lu B\n\n",    totalBytes_ );

   printf("Term count: %lu\n",       totalTokenCount_);
   printf("Mean term length: %7.2f\n",  totalTokenLength_ / float(totalTokenCount_) );

   printf("Speed: %7.2f doc/s\n",      fileNames_.size() / (totalDuration_/1000.)  );
   printf("Speed: %7.2f kB/s\n",    totalBytes_/1000. / (totalDuration_/1000.) );
   printf("====================================\n");
}

unsigned long Storage::totalTokenSize() const
{
  return totalTokenLength_;
}

void Storage::setTotalTokenSize(unsigned long totalTokenSize)
{
  totalTokenLength_ = totalTokenSize;
}

size_t Storage::size() const
{
  return table_.size();
}

const WString &Storage::dir() const
{
  return dir_;
}

void Storage::setDir(const WString &dir)
{
  dir_ = dir;
}
//-------------------------------------------------------------------------------------------
