#include "Storage.h"
#include "Encoding.h"
#include "Utils.h"

#include <stdio.h> // for printf
//-------------------------------------------------------------------------------------------
//  Serialize
//-------------------------------------------------------------------------------------------
void serialize(size_t value,File &file)
{
  file.write(&value,sizeof(value));
}

void deserialize(size_t& value,File &file)
{
  file.read(&value,sizeof(value));
}
//-------------------------------------------------------------------------------------------
void serialize(const WString& wstr,File &file)
{
  serialize(wstr.size(),file);
  file.write(wstr.data(),wstr.size()*sizeof(WString::value_type));
}

void deserialize(WString& wstr,File &file)
{
  size_t size= 0;
  deserialize(size,file);
  wstr.resize(size);

  file.read(const_cast<wchar_t*>(wstr.data()),size*sizeof(WString::value_type));
}
//-------------------------------------------------------------------------------------------
void serialize(const WStrings& wstrs,File &file)
{
  serialize(wstrs.size(),file);
  for(size_t i=0; i<wstrs.size(); ++i)
    serialize(wstrs[i],file);
}

void deserialize(WStrings& wstrs,File &file)
{
  size_t size=0;
  deserialize(size,file);
  wstrs.resize(size);

  for(size_t i=0; i<size; ++i)
    deserialize(wstrs[i],file);
}
//-------------------------------------------------------------------------------------------
void serialize(const Location& dirrection,File &file)
{
   serialize(dirrection.fileIndex,file);
   serialize(dirrection.tokenNumber,file);
   serialize(dirrection.pos,file);
   //serialize(dirrection.line,file);
   //serialize(dirrection.posInLine,file);
}

void deserialize(Location& dirrection,File &file)
{
  deserialize(dirrection.fileIndex,file);
  deserialize(dirrection.tokenNumber,file);
  deserialize(dirrection.pos,file);
  //deserialize(dirrection.line,file);
  //deserialize(dirrection.posInLine,file);
}
//-------------------------------------------------------------------------------------------
void serialize(const Locations& locations,File &file)
{
  serialize(locations.size(),file);
  for(const auto& location:locations)
     serialize(location,file);
}

void deserialize(Locations& locations,File &file)
{
  size_t size=0;
  deserialize(size,file);
  for(size_t i=0; i<size; ++i)
  {
     Location location;
     deserialize(location,file);
     locations.push_back(location);
  }
}
//-------------------------------------------------------------------------------------------
//    Storage
//-------------------------------------------------------------------------------------------
Storage::Storage(size_t bucket_size)
  :table_(bucket_size)
{
  fileNames_.reserve(40000);
  snippets_.reserve(40000);
}

bool Storage::index(const WString &dir,size_t maxFileCount)
{
  fileNames_.clear();
  snippets_.clear();
  table_.clear();

  printf("Поиск файлов ...\n");
  const size_t count= entry(fileNames_,dir,L"*.txt",AllFiles);

  if(maxFileCount)
    fileNames_.resize(maxFileCount);

  printf("Обработка файлов ...\n");
  for(size_t f=0; f<fileNames_.size(); ++f)
  {
    size_t endOfSnippet= 0;

    const String printedFileName = wStringToBytes(fileNames_[f],CP_UTF8);
    printf("%d. \"%s\"\n",f+1,printedFileName.c_str());

    const WString path=  join(dir,fileNames_[f]);
    const Bytes   bytes= readAllText(path);
    WString text=  bytesToWString(bytes,CP_UTF8);

    Tokens tokens= tokenize(text);
    for(size_t t=0; t<tokens.size(); ++t)
    {
      tokens[t].location.fileIndex= f;
      table_[tokens[t].token].push_back(tokens[t].location);

      // Для сниппета берем чуть больше N символов не обрезая при этом слова
      if(endOfSnippet==0 && tokens[t].location.pos>=200)
        endOfSnippet= tokens[t].location.pos + tokens[t].token.size();
    }
    snippets_.push_back(text.substr(0,endOfSnippet));
  }
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
    deserialize(token, file); // token

    Locations locations;
    deserialize(locations,file); // locations

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

  serialize(table_.size(),file); // token size
  const auto& buckets = table_.buckets();
  for(const auto& bucket:buckets)
  {
    if(bucket)
    {
      for(const auto& pair: *bucket)
      {
        serialize(pair.first,file);  // token
        serialize(pair.second,file); // locations
      }
    }
  }
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
