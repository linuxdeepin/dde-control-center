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
    property bool isSiderNavigate: false

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
    property var navigateIconModel: ListModel {}
    property real trayIconHeight: {
        if(navigateIconModel.count==0){
            return trayWidth
        }
        else{
            var new_tray_height = iconsArea.height/(navigateIconModel.count)
            return new_tray_height < trayWidth ? new_tray_height : trayWidth
        }
    }

    function initTrayIcon() {
        print("==> initTrayIcon emit")
        avatarImage.imageSource = currentUserObj.iconFile
        userName.text = currentUserObj.userName.substring(0, 1).toUpperCase() + currentUserObj.userName.substring(1)
        var modules_id_array = modulesId.allIds
        moduleIconList.currentIndex = -1
        navigateIconModel.clear()
        for (var i in modules_id_array){
            var module_id = modules_id_array[i]
            if(module_id != "touchpad"){
                navigateIconModel.append({
                    "moduleId": module_id,
                })
            }
            else if(module_id == "touchpad" && typeof inputDevices[module_id] != "undefined"){
                navigateIconModel.append({
                    "moduleId": module_id,
                })
            }
        }
        iconIdToIndex = new Object()
        for(var i=0; i<navigateIconModel.count; i++){
            iconIdToIndex[navigateIconModel.get(i).moduleId] = i
        }
    }

    function addHomeShutdownButton() {
        navigateIconModel.insert(0, { "moduleId": "home" })
        navigateIconModel.append({ "moduleId": "shutdown" })
        panelContent.isSiderNavigate = true
    }

    function removeHomeShutdownButton() {
        navigateIconModel.remove(0)
        navigateIconModel.remove(navigateIconModel.count - 1)
        panelContent.isSiderNavigate = false
    }

    function shutdownButtonClicked(){
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

    function trayIconHoverHandler(module_id, index) {
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayIconHeight - trayIconTip.height)/2 + trayIconHeight * index
        if (trayIconHeight == trayWidth) {
            //tipDisplayHeight += (panelContent.height - trayIconHeight * navigateIconModel.count)/2
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
        to: navigateIconModel.count * trayIconHeight

        onStopped: {
            inExpandHideTrayIcon = false
        }
    }

    Binding {
        target: moduleIconList
        property: "height"
        value: navigateIconModel.count * trayIconHeight
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

    SequentialAnimation {
        id: toSiderNavigateAnimation

        onStopped: addHomeShutdownButton()

        ParallelAnimation {
            PropertyAnimation {
                target: headerArea
                property: "y"
                to: 0 - headerArea.height
                duration: 300
            }

            PropertyAnimation {
                target: footerArea
                property: "anchors.bottomMargin"
                to: -80
                duration: 300
            }

        }

        ParallelAnimation {
            PropertyAnimation {
                target: iconsArea
                property: "width"
                to: 48
                duration: 300
            }

            PropertyAnimation {
                target: iconsArea
                property: "anchors.topMargin"
                to: 0
                duration: 300
            }

            PropertyAnimation {
                target: listBox
                property: "width"
                to: 48
                duration: 300
            }

            PropertyAnimation {
                target: moduleIconList
                property: "iconLabelOpacity"
                to: 0
                duration: 300
            }

            PropertyAnimation {
                target: moduleIconList
                property: "itemSize"
                to: 48
                duration: 300
            }
        }
    }

    SequentialAnimation {
        id: toGridNavigateAnimation

        onStarted: removeHomeShutdownButton()

        ParallelAnimation {
            PropertyAnimation {
                target: iconsArea
                property: "width"
                to: panelContent.width
                duration: 300
            }

            PropertyAnimation {
                target: iconsArea
                property: "anchors.topMargin"
                to: headerArea.height
                duration: 300
            }

            PropertyAnimation {
                target: listBox
                property: "width"
                to: 96 * 3
                duration: 300
            }

            PropertyAnimation {
                target: moduleIconList
                property: "iconLabelOpacity"
                to: 1
                duration: 300
            }

            PropertyAnimation {
                target: moduleIconList
                property: "itemSize"
                to: 96
                duration: 300
            }
        }

        ParallelAnimation {
            PropertyAnimation {
                target: headerArea
                property: "y"
                to: 0
                duration: 300
            }

            PropertyAnimation {
                target: footerArea
                property: "anchors.bottomMargin"
                to: 0
                duration: 300
            }
        }
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
        anchors.top: parent.top
        anchors.topMargin: headerArea.height
        anchors.left: parent.left
        width: parent.width
        height: childrenRect.height

        Item {
            id: listBox
            width: 96 * 3
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter

            GridView {
                id: moduleIconList
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                property int itemSize: 96
                cellHeight: itemSize
                cellWidth: itemSize
                property bool isSiderNavigate: false
                property real iconLabelOpacity: 1

                function iconClickAction(index, iconId) {
                    if (iconId == 'shutdown'){
                        shutdownButtonClicked()
                    }
                    else if(iconId == "home"){
                        toGridNavigateAnimation.start()
                    }
                    else{
                        if (frame.x != rootWindow.width - panelWidth){
                            showAll.restart()
                        }
                        rightBoxLoaderItem.iconId = iconId
                        if( rightBox.visible == false ) { 
                            rightBox.visible = true
                            rightBox.opacity = 1
                        }
                        inDssHome = false
                        if(!panelContent.isSiderNavigate){
                            toSiderNavigateAnimation.start()
                        }
                    }
                }

                delegate: ModuleIconItem {}
                model: navigateIconModel
                currentIndex: -1
                maximumFlickVelocity: 0
            }
        }
    }

    Item {
        id: footerArea
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
                shutdownButtonClicked()
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
        anchors.left: iconsArea.right
        clip: true

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            //Eats mouse events
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
