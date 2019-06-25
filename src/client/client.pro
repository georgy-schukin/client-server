TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

TARGET = client

LIBS += -lboost_system -pthread

SOURCES += \
    client.cpp

