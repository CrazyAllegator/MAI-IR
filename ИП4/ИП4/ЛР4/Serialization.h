#ifndef SERIALIZATION_H
#define SERIALIZATION_H
//--------------------------------------------------
#include "Global.h"
#include "FS.h"
//--------------------------------------------------
void serialize(size_t value,File &file);
void deserialize(size_t& value,File &file);

void serialize(const WString& wstr,File &file);
void deserialize(WString& wstr,File &file);

void serialize(const WString& wstr,File &file);
void deserialize(WString& wstr,File &file);

void serialize(const WStrings& wstrs,File &file);
void deserialize(WStrings& wstrs,File &file);

void serialize(const Location& dirrection,File &file);
void deserialize(Location& dirrection,File &file);

void serialize(const Locations& locations,File &file);
void deserialize(Locations& locations,File &file);
//--------------------------------------------------
#endif // SERIALIZATION_H
