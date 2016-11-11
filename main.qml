import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Window {
    visible: true
    width: 640
    height: 480
    color: "#323232"

    MainForm {
        progress.onValueChanged: {
            if(progress.value == 0.0 || progress.value == 1.0)
                progressBar1.visible = false;
        }
        anchors.fill: parent
    }
}
