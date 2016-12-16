TEMPLATE = subdirs

DEFINES += QT_NOT_DEBUG_OUTPUT

SUBDIRS += github-cli github-qml libgithub

github-cli.depends += libgithub
github-qml.depends += libgithub
