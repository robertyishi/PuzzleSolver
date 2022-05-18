TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        mmz.cpp \
        move.cpp \
        position.cpp \
        puzzle.cpp \
        solver.cpp \
        toh.cpp

HEADERS += \
    mmz.h \
    move.h \
    position.h \
    puzzle.h \
    solver.h \
    toh.h
