import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Row {
    id: panelContent
    width: parent.width
    height: parent.height

    property alias moduleLoaderItem: rightBoxLoaderItem

    property var iconIdToIndex

    property var modulesId: ModulesData {}
    property bool inExpandHideTrayIcon: false
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
        var modules_id_array = modulesId.commonIds
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
        //console.log(module_id + ": "  + index)
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayIconHeight - trayIconTip.height)/2 + trayIconHeight * index
        if (trayIconHeight == trayWidth) {
            tipDisplayHeight += (panelContent.height - trayIconHeight * trayIconModel.count)/2
        }
        trayIconTip.y = tipDisplayHeight
        trayIconTip.text = modulesId.moduleLocaleNames[module_id]
        //trayIconTip.opacity = 1.0
        trayIconTip.visible = true
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
        width: trayWidth
        height: parent.height

        Rectangle {
            color: dconstants.bgColor
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            ListView {
                id: trayIconList
                width: parent.width
                //height: childrenRect.height
                anchors.centerIn: parent

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
                    }
                }

                delegate: ModuleIconItem {
                    width: trayWidth
                    height: trayIconHeight
                }
                model: ListModel { id: trayIconModel }
                currentIndex: -1
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 2; }
                highlightMoveDuration: 300
                highlightFollowsCurrentItem: true
                maximumFlickVelocity: 0
            }
        }
    }

    DSeparatorVertical {}

    Item {
        id: rightBox
        width: parent.width - trayWidth - 2
        height: parent.height

        Item {
            id: rightBoxLoaderItem

            property string iconId

            visible: false
            clip: true
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
            anchors.fill: parent

            MouseArea{
                anchors.fill: parent
                enabled: rightBoxLoaderItem.visible
                //Eats mouse events
            }

            Loader{
                id: rightBoxLoader
                property string iconId
                focus: true
                source: ''
                anchors.fill: parent
            }
        }
    }
}
