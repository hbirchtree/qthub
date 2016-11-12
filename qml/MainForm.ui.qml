import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

Rectangle {
    id: rectangle1

    width: 800
    height: 600
    color: "#00000000"
    property alias button1: button1
    property alias progress: progress

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
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    spacing: 10
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                    }

                    Text {
                        text: name
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                    }
                }
            }
            model: ListModel {
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }
            }
        }

        Rectangle {
            id: rectangle2
            color: "#00000000"

            Button {
                id: button1
                x: 8
                y: 8
                text: qsTr("Button")
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
