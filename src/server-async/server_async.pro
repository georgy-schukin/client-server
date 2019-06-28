TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

TARGET = server_async

LIBS += -lboost_system -pthread

SOURCES += \
    server_async.cpp

