TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Encoding.cpp \
        Exception.cpp \
        FS.cpp \
        Tokenizing.cpp \
        main.cpp

HEADERS += \
  Encoding.h \
  Exception.h \
  FS.h \
  Global.h \
  Tokenizing.h \
  UserString.h \
  UserVector.h \
  Utils.h


LIBS+= -lShlwapi -lShell32
