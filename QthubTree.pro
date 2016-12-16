TEMPLATE = subdirs

INCLUDEPATH += libgithub/include

SUBDIRS += github-cli github-qml libgithub

github-cli.depends += libgithub
github-qml.depends += libgithub
