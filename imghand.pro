TEMPLATE = app
# QT += core widgets printsupport
QT += core widgets
TARGET = imghand
CONFIG += c++17 release warn_on qt thread
QMAKE_CXXFLAGS += -g3  -std=gnu++17
OBJECTS_DIR=.obj
MOC_DIR=.moc

# FORMS += xx.ui

SOURCES += imghand.cpp \
           main.cpp

HEADERS += imghand.h

RESOURCES = imghand.qrc

