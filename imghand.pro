TEMPLATE = app
# QT += core widgets printsupport
QT += core widgets charts
TARGET = imghand
CONFIG += c++17 release warn_on qt thread
QMAKE_CXXFLAGS += -g3  -std=gnu++17
OBJECTS_DIR=.obj
MOC_DIR=.moc

unix:LIBS += $$system(gsl-config --libs)
win32:LIBS += -lgsl -lgslcblas -lm

# FORMS += xx.ui

SOURCES += imghand.cpp \
           main.cpp

HEADERS += imghand.h

RESOURCES = imghand.qrc

