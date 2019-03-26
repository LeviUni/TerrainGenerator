#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T12:26:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGL_transformations
TEMPLATE = app
CONFIG += c++14

SOURCES += main.cpp\
    mainwindow.cpp \
    mainview.cpp \
    user_input.cpp \
    model.cpp \
    modeldata.cpp \
    shaderprogram.cpp \
    material.cpp \
    object.cpp \
    noisegrid.cpp \
    framebuffer.cpp \
    texture.cpp \
    renderbuffer.cpp

HEADERS  += mainwindow.h \
    mainview.h \
    model.h \
    vertex.h \
    modeldata.h \
    shaderprogram.h \
    material.h \
    object.h \
    noisegrid.h \
    framebuffer.h \
    texture.h \
    renderbuffer.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES +=
