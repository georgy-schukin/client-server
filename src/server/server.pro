TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

TARGET = server

LIBS += -lboost_system -pthread

SOURCES += \
    server.cpp

