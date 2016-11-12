TEMPLATE = app

QT += qml quick network
CONFIG += c++11

SOURCES += main.cpp \
    github/githubuser.cpp \
    github/githubrepo.cpp \
    githubfetch.cpp \
    github/githubrelease.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    github/githubuser.h \
    github/githubrepo.h \
    githubfetch.h \
    github/githubrelease.h
