QT += core gui widgets charts
CONFIG += c++17 console
TEMPLATE = app
TARGET = RailwaySystemGUI

SOURCES += Station.cpp \
           Route.cpp \
           Train.cpp \
           PassengerFlow.cpp \
           DataAnalyzer.cpp \
           FileManager.cpp \
           AdvancedAnalyzer.cpp \
           TimeSeriesAnalyzer.cpp \
           main_gui.cpp

HEADERS += Station.h \
           Route.h \
           Train.h \
           PassengerFlow.h \
           DataAnalyzer.h \
           FileManager.h \
           AdvancedAnalyzer.h \
           TimeSeriesAnalyzer.h

DESTDIR = bin

win32 {
    QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
    LIBS += -lmingw32
    LIBS += $$[QT_INSTALL_LIBS]/libQt6EntryPoint.a
    LIBS += -lshell32 -lkernel32 -luser32 -lgdi32 -lcomdlg32 -ladvapi32
}
