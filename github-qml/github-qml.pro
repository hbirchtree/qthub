TEMPLATE = app

# Configuration
QT += qml quick network
CONFIG += c++11 link_prl

SOURCES += main.cpp
INCLUDEPATH += ../libgithub/include

LIBS += -L../libgithub -lgithub
RESOURCES += qml/qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
