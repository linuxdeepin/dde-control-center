import DBus.Com.Deepin.Daemon.ExtDevManager 1.0
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22

    ExtDevManager {
       id: "extDevManagerID"
       // path: "/com/deepin/daemon/ExtDevManager"
    } 
    Keyboard {
       id: "keyboardID"
       // path: "/com/deepin/daemon/ExtDevManager/Keyboard"
    } 
    Mouse {
       id: "mouseID"
       // path: "/com/deepin/daemon/ExtDevManager/Mouse"
    } 
    TouchPad {
       id: "touchPadID"
       // path: "/com/deepin/daemon/ExtDevManager/TouchPad"
    } 
    Text{
	    text: JSON.stringify(extDevManagerID.devInfoList)
    }
}
