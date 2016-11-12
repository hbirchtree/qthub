import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import Qthub 1.0
import Qthub.daemon 1.0

Window {
    visible: true
    width: 640
    height: 480
    color: "#323232"

    function test(user)
    {
        console.log(user, user.login)
    }

    MainForm {
        button1.onClicked: {
            HubDaemon.userUpdated.connect(test)
            HubDaemon.fetchUser("hbirchtree");
        }
        progress.onValueChanged: {
            if(progress.value == 0.0 || progress.value == 1.0)
                progressBar1.visible = false;
        }
        anchors.fill: parent
    }
}
