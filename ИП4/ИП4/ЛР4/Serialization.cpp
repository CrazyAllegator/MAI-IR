#include "Serialization.h"
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
}

void deserialize(Location& dirrection,File &file)
{
  deserialize(dirrection.fileIndex,file);
  deserialize(dirrection.tokenNumber,file);
  deserialize(dirrection.pos,file);
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
