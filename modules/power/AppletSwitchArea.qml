import QtQuick 2.0
import Deepin.Widgets 1.0
import DBus.Dde.Dock.Entry.AppletManager 1.0

Column {
    width: parent.width

    property var dbusAppletManager: AppletManager {}
    property var appletInfos: JSON.parse(dbusAppletManager.appletInfoList)
    property string appletId: "power"
    property bool appletDisplay: {
        for(var i in appletInfos){
            var info = appletInfos[i]
            if(info[0] == appletId){
                return info[2]
            }
        }
        return False
    }

    DSwitchButtonHeader {
        text: dsTr("Dock Applet Enabled")
        active: appletDisplay
        onClicked: {
            if(active){
                dbusAppletManager.ShowApplet(appletId)
            }
            else{
                dbusAppletManager.HideApplet(appletId)
            }
        }
    }

    DSeparatorHorizontal {}
}
