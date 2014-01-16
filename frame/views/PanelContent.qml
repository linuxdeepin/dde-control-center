import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Row {
    id: panelContent

    property alias moduleLoaderItem: rightBoxLoaderItem
    property alias moduleBox: rightBox
    property var iconIdToIndex

    property var modulesId: ModulesData {}
    property bool inExpandHideTrayIcon: false
    property bool inDssHome: true
    property var trayIconModel: ListModel {}
    property real trayIconHeight: {
        if(trayIconModel.count==0){
            return trayWidth
        }
        else{
            var new_tray_height = trayArea.height/(trayIconModel.count)
            return new_tray_height < trayWidth ? new_tray_height : trayWidth
        }
    }

    function initTrayIcon() {
        print(">>>>> initTrayIcon emit")
        var modules_id_array = modulesId.allIds
        trayIconList.currentIndex = -1
        trayIconModel.clear()
        for (var i in modules_id_array){
            var module_id = modules_id_array[i]
            trayIconModel.append({
                "moduleId": module_id,
            })
        }
        iconIdToIndex = new Object()
        for(var i=0; i<trayIconModel.count; i++){
            iconIdToIndex[trayIconModel.get(i).moduleId] = i
        }
    }

    function expandHideTrayIcon() {
        print(">>>>> expandHideTrayIcon emit")
        inExpandHideTrayIcon = true
        trayIconModel.remove(trayIconModel.count - 2)
        trayIconList.currentIndex = -1
        var newIds = modulesId.hideIds
        for (var i in newIds){
            var index = trayIconModel.count - 1
            var module_id = newIds[i]
            trayIconModel.insert(index, {
                "moduleId": module_id,
            })
        }
        expandHideTrayIconAnimation.start()
        iconIdToIndex = new Object()
        for(var i=0; i<trayIconModel.count; i++){
            iconIdToIndex[trayIconModel.get(i).moduleId] = i
        }
    }

    function trayIconHoverHandler(module_id, index) {
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayIconHeight - trayIconTip.height)/2 + trayIconHeight * index
        if (trayIconHeight == trayWidth) {
            tipDisplayHeight += (panelContent.height - trayIconHeight * trayIconModel.count)/2
        }
        trayIconTip.y = tipDisplayHeight
        trayIconTip.text = modulesId.moduleLocaleNames[module_id]
        if(!inDssHome){
            trayIconTip.visible = true
        }
    }

    NumberAnimation {
        id: expandHideTrayIconAnimation
        duration: 300
        target: trayIconList
        properties: "height"
        to: trayIconModel.count * trayIconHeight

        onStopped: {
            inExpandHideTrayIcon = false
        }
    }

    Binding {
        target: trayIconList
        property: "height"
        value: trayIconModel.count * trayIconHeight
        when: !inExpandHideTrayIcon
    }

    Timer {
        running: true
        repeat: false
        interval: 100
        onTriggered: {
            initTrayIcon()
        }
    }

    Item {
        id: trayArea
        width: parent.width
        height: parent.height

        Item {
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            ListView {
                id: trayIconList
                width: parent.width
                //height: childrenRect.height
                //anchors.centerIn: parent
                contentHeight: height

                function iconClickAction(trayIconId) {
                    if (trayIconId == 'shutdown'){
                        windowView.shutdown()
                        hideAll.restart()
                        if(rightBoxLoaderItem.iconId != ''){
                            trayIconList.currentIndex = iconIdToIndex[rightBoxLoaderItem.iconId]
                        }
                        else{
                            trayIconList.currentIndex = -1
                        }
                    }
                    else if (trayIconId == 'dss'){
                        expandHideTrayIcon()
                    }
                    else{
                        if (frame.x != rootWindow.width - panelWidth){
                            showAll.restart()
                        }
                        rightBoxLoaderItem.iconId = trayIconId
                        if( rightBox.x == panelContent.width ) { 
                            rightBox.x = trayWidth
                        }
                        inDssHome = false
                    }
                }

                delegate: ModuleIconItem {
                    width: parent.width
                    height: trayIconHeight
                }
                model: trayIconModel
                currentIndex: -1
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 2; }
                highlightMoveDuration: 300
                highlightFollowsCurrentItem: true
                maximumFlickVelocity: 0
            }
        }
    }

    Rectangle {
        id: rightBox
        width: parent.width - trayWidth
        height: parent.height
        color: dconstants.bgColor
        x: parent.width
        clip: true

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            //Eats mouse events
        }

        Behavior on x {
            SmoothedAnimation { 
                duration: 300 
                easing.type: Easing.InQuart
            }
        }

        Row {
            id: rightBoxLoaderItem
            width: parent.width
            height: parent.height
            clip: true

            property string iconId
            onIconIdChanged: {
                rightBoxLoaderItem.visible = (iconId == '' ? false : true)
                rightBoxLoader.iconId = iconId
                if (iconId){
                    rightBoxLoader.source = '../../modules/' + iconId + '/main.qml'
                }
                else{
                    rightBoxLoader.source = ''
                }
            }

            DSeparatorVertical {}

            Loader{
                id: rightBoxLoader
                property string iconId
                focus: true
                clip: true
                source: ''
                width: parent.width - 2
                height: parent.height
            }
        }
    }
}
