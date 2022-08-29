TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        lightsout.cpp \
        main.cpp \
        mmz.cpp \
        move.cpp \
        optsolver.cpp \
        position.cpp \
        puzzle.cpp \
        solver.cpp \
        ternary.cpp \
        toh.cpp

HEADERS += \
    lightsout.h \
    mmz.h \
    move.h \
    optsolver.h \
    position.h \
    puzzle.h \
    solver.h \
    ternary.h \
    toh.h
