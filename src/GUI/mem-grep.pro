QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mem-grep-gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -std=c++17 -g -Wall -Wextra -Wreorder -Weffc++ -Wpedantic -Wimplicit-fallthrough

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        parsing.cpp \
        ../shared/filtering/lambda-creator.cpp \
        ../shared/filtering/heap-filter.cpp \
        ../shared/misc/remote-memory.cpp \
        ../shared/misc/map-parser.cpp \
        ../shared/misc/utils.cpp \
        ../shared/misc/prerun-checks.cpp \
        ../shared/heap-traversing/bss-searcher.cpp \
        ../shared/heap-traversing/stack-searcher.cpp \
        ../shared/heap-traversing/heap-traverser.cpp \
        ../shared/misc/structs.cpp \
        ../shared/misc/malloc-metadata.cpp \
        ../shared/analyse-program.cpp

HEADERS += \
        parsing.hpp \
        mainwindow.hpp \
        ../shared/filtering/lambda-creator.hpp \
        ../shared/filtering/heap-filter.hpp \
        ../shared/misc/remote-memory.hpp \
        ../shared/misc/map-parser.hpp \
        ../shared/misc/utils.hpp \
        ../shared/misc/prerun-checks.hpp \
        ../shared/heap-traversing/bss-searcher.hpp \
        ../shared/heap-traversing/stack-searcher.hpp \
        ../shared/heap-traversing/heap-traverser.hpp \
        ../shared/misc/structs.hpp \
        ../shared/misc/malloc-metadata.hpp \
        ../shared/analyse-program.hpp

FORMS += \
        mainwindow.ui
