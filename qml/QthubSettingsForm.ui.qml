import QtQuick 2.4
import QtQuick.Controls 1.5

Item {
    id: item1
    width: 400
    height: 400

    SplitView {
        id: splitView1
        anchors.fill: parent

        ListView {
            id: listView1
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
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
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
                    spacing: 10
                }
            }
        }

        Rectangle {
            id: rectangle1
            x: 127
            y: 26
            width: 200
            height: 200
            color: "#ffffff"
        }
    }
}
