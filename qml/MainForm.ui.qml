import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    id: rectangle1

    width: 800
    height: 600
    color: "#00000000"
    property alias button1: button1
    property alias progress: progress
    property alias progressText: progressText

    property alias repoView: repoView
    property alias repoList: repoList

    SplitView {
        id: splitView1
        anchors.bottom: progress.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 6

        ListView {
            id: repoList
            width: 218
            height: 600
            delegate: Repository {}
        }

        Rectangle {
            id: rectangle2
            color: "#00000000"

            Button {
                id: button1
                x: 29
                width: 133
                height: 20
                text: qsTr("Button")
                anchors.top: parent.top
                anchors.topMargin: 437
            }

            Repository {
                id: repoView
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
            }
        }
    }

    ProgressBar {
        id: progress
        y: 569
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 8

        Text {
            id: progressText
            text: qsTr("Text")
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }
    }
}
