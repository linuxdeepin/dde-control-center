import QtQuick 2.0
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import "../widgets"

DockApplet{
    title: "Disk Mount"
    appid: "AppletDiskMount"
    icon: iconPath

    property url iconPath: "images/icon.png"
    property int xEdgePadding: 0

    onActivate:{
        showDiskMount()
    }

    function showDiskMount(){
        dbusControlCenter.ShowModule("default_applications")
    }

    function hideDiskMount(){
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
        width: 240 + xEdgePadding * 2
        height: diskListView.height + xEdgePadding * 2
        color: Qt.rgba(0, 0, 0, 0.85)

        ListView {
            id: diskListView
            width: parent.width
            height: childrenRect.height
            model: mountDiskList
            delegate: DiskItem {}
        }
    }
}
