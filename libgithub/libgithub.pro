QT += network testlib
QT -= gui

TARGET = github
TEMPLATE = lib
CONFIG += c++11 staticlib create_prl

SOURCES += \
    src/githubfetch.cpp \
    src/githubfetch_object_builders.cpp \
    src/githubfetch_request_handlers.cpp \
    src/githubfetch_network_handlers.cpp \
    src/githubrelease.cpp \
    src/githubrepo.cpp \
    src/githubuser.cpp \
    src/githubtag.cpp \
    src/githubreleasefile.cpp \
    src/githubbranch.cpp \
    src/githubcommit.cpp

HEADERS += \
    include/github/githubfetch.h \
    include/github/githubrelease.h \
    include/github/githubrepo.h \
    include/github/githubuser.h \
    include/github/githubtag.h \
    include/github/githubreleasefile.h \
    include/github/githubbranch.h \
    include/github/githubcommit.h

INCLUDEPATH += include

unix {
    target.path = /usr/lib
    INSTALLS += target
}
