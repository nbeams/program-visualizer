include(../../config.pri)
TEMPLATE = lib
TARGET = 
DEPENDPATH += src
INCLUDEPATH += ../../external/include \
    ../../editor/src

CONFIG -= debug \
    debug_and_release
CONFIG += qt \
    staticlib \
    release
QT += svg
win32:INCLUDEPATH += QSci \
    CONFIG \
    -= \
    embed_manifest_exe
LIBS += -lqscintilla2
win32:LIBS += -lshell32

# Input
HEADERS += src/ProgramVisualizer.h \
    src/FlowchartItem.h \
    src/BrowserItem.h \
    src/VisualizerGraphicsScene.h \
    src/FunctionItem.h
SOURCES += src/ProgramVisualizer.cpp \
    src/FlowchartItem.cpp \
    src/BrowserItem.cpp \
    src/VisualizerGraphicsScene.cpp \
    src/FunctionItem.cpp
FORMS += ui/ProgramVisualizer.ui

RESOURCES += rc/program_visualizer.qrc
