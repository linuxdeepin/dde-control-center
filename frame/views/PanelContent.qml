import QtQuick 2.1
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import DBus.Com.Deepin.SessionManager 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0

Rectangle {
    id: panelContent
    color: dconstants.bgColor

    property alias moduleLoaderItem: rightBoxLoaderItem
    property alias moduleBox: rightBox
    property var iconIdToIndex
    property color tuhaoColor: "#faca57"

    property var sessionManager: SessionManager {}
    property var modulesId: ModulesData {}
    property var accountId: Accounts {}
    property var currentUserObj: User { path: accountId.ListCachedUsers()[0] }

    property bool inExpandHideTrayIcon: false
    property bool inDssHome: true
    property var trayIconModel: ListModel {}
    property real trayIconHeight: {
        if(trayIconModel.count==0){
            return trayWidth
        }
        else{
            var new_tray_height = iconsArea.height/(trayIconModel.count)
            return new_tray_height < trayWidth ? new_tray_height : trayWidth
        }
    }

    function initTrayIcon() {
        print(">>>>> initTrayIcon emit")
        avatarImage.imageSource = currentUserObj.iconFile
        userName.text = currentUserObj.userName.substring(0, 1).toUpperCase() + currentUserObj.userName.substring(1)
        var modules_id_array = modulesId.allIds
        moduleIconList.currentIndex = -1
        trayIconModel.clear()
        for (var i in modules_id_array){
            var module_id = modules_id_array[i]
            if(module_id != "touchpad"){
                trayIconModel.append({
                    "moduleId": module_id,
                })
            }
            else if(module_id == "touchpad" && typeof inputDevices[module_id] != "undefined"){
                trayIconModel.append({
                    "moduleId": module_id,
                })
            }
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
        moduleIconList.currentIndex = -1
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
            //tipDisplayHeight += (panelContent.height - trayIconHeight * trayIconModel.count)/2
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
        target: moduleIconList
        properties: "height"
        to: trayIconModel.count * trayIconHeight

        onStopped: {
            inExpandHideTrayIcon = false
        }
    }

    Binding {
        target: moduleIconList
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

    states: [
        State {
            name: "siderNavigate"
            PropertyChanges {target: moduleIconList; itemSize: 48}
            PropertyChanges {target: iconsArea; width: 50; anchors.leftMargin: 0}
            PropertyChanges {target: headerArea; y: 0 - headerArea.height;}
            PropertyChanges {target: bottomArea; anchors.bottomMargin: -80;}
        }
    ]

    function changeToSiderNavigate(){
    }

    function returnToHomeNavigate(){
    }

    Column {
        id: headerArea
        height: childrenRect.height
        width: parent.width

        Rectangle {
            width: parent.width
            height: 150
            color: dconstants.contentBgColor

            DRoundImage {
                id: avatarImage
                roundRadius: 40
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
            }

            DLabel {
                id: userName
                anchors.top: avatarImage.bottom
                anchors.topMargin: 6
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 22
                color: tuhaoColor
            }
        }

        DSeparatorHorizontal{}

        DBaseLine {}
    }

    Item {
        id: iconsArea
        anchors.top: headerArea.bottom
        anchors.left: parent.left
        anchors.leftMargin: (parent.width - width) / 2
        width: 96 * 3
        height: childrenRect.height

        GridView {
            id: moduleIconList
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            property int itemSize: 96
            cellHeight: itemSize
            cellWidth: itemSize

            function iconClickAction(index, trayIconId) {
                if (trayIconId == 'shutdown'){
                    rootWindow.displayWidth = 0
                    rootWindow.hide()
                    if(rightBoxLoaderItem.iconId != ''){
                        moduleIconList.currentIndex = iconIdToIndex[rightBoxLoaderItem.iconId]
                        resetTimer.restart()
                    }
                    else{
                        moduleIconList.currentIndex = -1
                        initTrayIcon()
                    }
                    sessionManager.PowerOffChoose()
                }
                else{
                    if (frame.x != rootWindow.width - panelWidth){
                        showAll.restart()
                    }
                    rightBoxLoaderItem.iconId = trayIconId
                    if( rightBox.visible == false ) { 
                        rightBox.visible = true
                        rightBox.opacity = 1
                    }
                    inDssHome = false
                    panelContent.state = "siderNavigate"
                }
            }

            delegate: ModuleIconItem {
                property bool isSiderNavigate: panelContent.state == "siderNavigate"
            }
            model: trayIconModel
            currentIndex: -1
            maximumFlickVelocity: 0
            //displaced: Transition {
                //NumberAnimation { properties: "x,y"; duration: 600 }
            //}
        }
    }

    Item {
        id: bottomArea
        width: parent.width
        anchors.bottom: parent.bottom

        Behavior on anchors.bottomMargin {
            NumberAnimation { duration: 300 }
        }

        Image {
            anchors.bottom: parent.bottom
            source: "images/shutdown_bg.png"
        }

        DImageButton {
            id: shutdownButton
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 36
            normal_image: "images/shutdown_normal.png"
            hover_image: "images/shutdown_hover.png"
            press_image: hover_image

            onStateChanged: {
                if(state == ""){
                    shutdownText.color = dconstants.fgColor
                } else {
                    shutdownText.color = tuhaoColor
                }
            }

            onClicked: {
                sessionManager.PowerOffChoose()
            }
        }

        DLabel {
            id: shutdownText
            text: dsTr("Shutdown")
            anchors.top: shutdownButton.bottom
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
        }
    }

    Rectangle {
        id: rightBox
        width: parent.width - trayWidth
        height: parent.height
        color: dconstants.bgColor
        x: 50
        clip: true
        visible: false
        opacity: 0

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            //Eats mouse events
        }

        Behavior on opacity {
            NumberAnimation { 
                duration: 1000
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
