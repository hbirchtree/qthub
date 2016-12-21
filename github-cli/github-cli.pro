TEMPLATE = app

# Configuration
QT -= gui
QT += network
CONFIG += c++11 link_prl console

SOURCES += main.cpp
INCLUDEPATH += ../libgithub/include

LIBS += -L../libgithub -lgithub

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
