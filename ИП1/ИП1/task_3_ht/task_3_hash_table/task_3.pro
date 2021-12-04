TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Encoding.cpp \
        Exception.cpp \
        FS.cpp \
        Storage.cpp \
        Tokenizing.cpp \
        main.cpp \
        murmur_hash2.cpp

HEADERS += \
  Encoding.h \
  Exception.h \
  FS.h \
  Global.h \
  Storage.h \
  Tokenizing.h \
  UserHashTable.h \
  UserList.h \
  UserString.h \
  UserVector.h \
  Utils.h \
  murmur_hash2.h


LIBS+= -lShlwapi
