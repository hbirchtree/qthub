import QtQuick 2.4
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

Item {
    id: item1
    width: 400
    height: 76

    property alias reponame: reponame
    property alias description: description
    property alias repotitle: repotitle
    property alias language: language

    Label {
        id: reponame
        text: qsTr("Label")
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8
    }

    Label {
        id: description
        text: qsTr("Label")
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.left: reponame.left
        anchors.leftMargin: 0
        anchors.top: reponame.bottom
        anchors.topMargin: 6
    }

    Label {
        id: label_spacer
        text: qsTr(">")
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: reponame.right
        anchors.leftMargin: 6
    }

    Label {
        id: repotitle
        text: qsTr("Label")
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: label_spacer.right
        anchors.leftMargin: 6
    }

    Label {
        id: language
        text: qsTr("Label")
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: repotitle.right
        anchors.leftMargin: 5
    }
}
