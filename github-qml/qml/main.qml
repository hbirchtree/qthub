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

    function userFound(user)
    {
        HubDaemon.fetchAllRepositories(user)
    }
    function repoFound(repo)
    {
        var repo_view = mf.repoList.addDisplaced()
        repo_view.setData(repo)
    }

    function authError()
    {
        console.log("Failed to authenticate with Github")
    }
    function networkError(code)
    {
        console.log("Network error occurred: ", code)
    }
    function showProgress(url, rec, total)
    {
        mf.progress.setValue(rec / total)
        mf.progressText.text = url
    }

    MainForm {
        id: mf

        button1.onClicked: {
            console.log("Network status: ", HubDaemon.status);
            HubDaemon.userUpdated.connect(userFound)
            HubDaemon.repoUpdated.connect(repoFound)
            HubDaemon.authenticationError.connect(authError)
            HubDaemon.networkReplyError.connect(networkError)
            HubDaemon.fetchUser("hbirchtree");

            HubDaemon.reportProgress.connect(showProgress);
        }
        progress.onValueChanged: {
            if(progress.value == 0.0 || progress.value == 1.0)
            {
                progress.visible = false;
                progress.height = 0;
            }else{
                progress.visible = true;
                progress.height = 24;
            }
        }
        anchors.fill: parent
    }
}
