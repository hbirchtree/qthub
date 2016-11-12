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

    property alias repoView: repoView

    SplitView {
        id: splitView1
        anchors.bottom: progress.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 6

        ListView {
            id: listView1
            width: 218
            height: 600
            delegate: Repository {
                description: r.description
                reponame: r.name
            }
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
    }

}
