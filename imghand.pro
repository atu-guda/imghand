TEMPLATE = app
# QT += core widgets printsupport
QT += core widgets charts
TARGET = imghand
CONFIG += c++17 release warn_on qt thread link_pkgconfig
PKGCONFIG += opencv
PKGCONFIG += gsl
QMAKE_CXXFLAGS += -g3  -std=gnu++17
OBJECTS_DIR=.obj
MOC_DIR=.moc

#unix:LIBS += $$system(gsl-config --libs)
# win32:LIBS += -lgsl -lgslcblas -lm

# FORMS += xx.ui

SOURCES += imghand.cpp
SOURCES += sobeldialog.cpp
SOURCES += main.cpp

HEADERS += imghand.h
HEADERS += sobeldialog.h

RESOURCES = imghand.qrc

