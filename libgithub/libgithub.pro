QT += network testlib

QT -= gui


TARGET = github
TEMPLATE = lib
CONFIG += c++11 staticlib create_prl

SOURCES += \
    src/githubfetch.cpp \
    src/githubrelease.cpp \
    src/githubrepo.cpp \
    src/githubuser.cpp

HEADERS += \
    include/github/githubfetch.h \
    include/github/githubrelease.h \
    include/github/githubrepo.h \
    include/github/githubuser.h

INCLUDEPATH += include

unix {
    target.path = /usr/lib
    INSTALLS += target
}
