import QtQuick 2.0
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets"

DockApplet{
    title: "Disk Mount"
    appid: "AppletDiskMount"
    icon: iconPath

    property url iconPath: getIconUrl("disk_mount/icon.png")
    property int windowWidth: 224
    property int xEdgePadding: 12

    onActivate:{
        showDiskMount(0)
    }

    function showDiskMount(id){
        dbusControlCenter.ShowModule("default_applications")
    }

    function hideDiskMount(id){
        set_hide_applet("disk_mount")
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showDiskMount);
            addItem("_Undock", hideDiskMount);
        }
    }

    function bitToHuman(b){
        b = b*1000
        var factor = 1000
        if (b < factor) {
            return b + " B"
        }
        else{
            b = b/factor
            if ( b < factor ){
                return b.toFixed(2) +" KB"
            }
            else {
                b = b/factor
                if (b < factor){
                    return b.toFixed(2) + " MB"
                }
                else{
                    b = b/factor
                    if(b<10){
                        return b.toFixed(2) + " GB"
                    }
                    else if(b<100){
                        return b.toFixed(1) + " GB"
                    }
                    else{
                        return parseInt(b) + " GB"
                    }
                }
            }
        }
    }

    window: DockQuickWindow {
        id: root
        width: windowWidth
        height: diskListView.height + xEdgePadding * 2
        color: "transparent"

        ListView {
            id: diskListView
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: xEdgePadding
            width: windowWidth
            height: Math.min(68 * mountDiskList.length, 300)
            model: mountDiskList
            clip: true
            delegate: DiskItem {
                width: windowWidth
            }

            DScrollBar {
                flickable: parent
            }
        }
    }
}
