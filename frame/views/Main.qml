import QtQuick 2.1
import QtQuick.Window 2.1
import Deepin.Locale 1.0
import "../../modules/widgets"

Item {
    id: root
    property var dconstants: DConstants {}

    property int trayWidth: 48
    property real trayHeight: trayWidth
    property color defaultBackgroundColor: dconstants.bgColor
    property bool trayIconShowAll: false
    property int viewHoverPadding: 100
    property int displayState: viewState.allHide
    property int lastDisplayState: viewState.allHide

    property bool clickedToHide: true

    DLocale {
        id: dsslocale
        domain: "deepin-system-settings"
        dirname: "../../locale"
    }

    function dsTr(s){
        return dsslocale.dsTr(s)
    }

    property var modulesId: ModulesData {}

    QtObject { 
        // enumeration for root view state
        id: viewState
        property int allHide: 0
        property int trayShow: 1
        property int rightBoxShow: 2
        property int allShow: 3
    }

    function outerAreaClicked(mousex, mousey){
        if (clickedToHide){
            if ((root.displayState == viewState.trayShow) && (
                mousex <= screenSize.width - trayWidth)) {
                hideTrayIcon()
            }
            else if ((root.displayState == viewState.rightBoxShow) && (
                mousex < screenSize.width - root.width +  48)) {
                hideTrayIcon()
            }
            else if ((root.displayState == viewState.allShow) && (
                mousex <= screenSize.width - root.width)) {
                hideTrayIcon()
            }
        }
    }

    function hideTrayIcon(){
        if (!hidingRoot.running){
            hidingRoot.restart()
        }
    }

    function displayTrayIcon(){
        trayFrame.visible = true
        if (lastDisplayState == viewState.allShow && timerResetState.running){
            timerResetState.stop()
            showingRightBox.restart()
        }
        else if (displayState == viewState.allHide && 
            !showingTrayIconBox.running){
            showingTrayIconBox.restart()
        }
    }

    function showModule(id){
        clickedToHide = false 
        trayFrame.visible = false
        showRightBox(id)
        displayState = viewState.rightBoxShow
    }

    function showRightBox(trayIconId) {
        if (trayIconId == 'shutdown'){
            //clickedToHide = !clickedToHide
            //Qt.quit()
            //hideTrayIcon()
            windowView.shutdown()
        }
        else if (trayIconId == 'dss'){
            expandHideTrayIcon()
        }
        else{
            if (!showingRightBox.running && !showingTrayIconBox.running){
                showingRightBox.restart()
            }
            rightBoxLoaderItem.iconId = trayIconId
        }
    }

    function initTrayIcon() {
        var modules_id_array = modulesId.commonIds
        var new_tray_height = root.height/(modules_id_array.length+1.0)
        if (new_tray_height < trayWidth){
            trayHeight = new_tray_height
        }
        else{
            trayHeight = trayWidth
        }
        trayIconTabArea.clear()
        for (var i in modules_id_array){
            var module_id = modules_id_array[i]
            trayIconTabArea.append({
                "moduleId": module_id,
                'statesArray': modulesId.getStatesArray(module_id)
            })
        }
        trayIconTabList.height = trayHeight * trayIconTabArea.count
        trayIconShowAll = false
    }

    function expandHideTrayIcon() {
        trayIconTabArea.remove(modulesId.commonIds.length - 2)
        trayIconTabList.currentIndex = -1
        var newIds = modulesId.hideIds
        for (var i in newIds){
            var index = trayIconTabArea.count - 1
            var module_id = newIds[i]
            trayIconTabArea.insert(index, {
                "moduleId": module_id,
                'statesArray': modulesId.getStatesArray(module_id)
            })
        }
        var new_tray_height = root.height/(trayIconTabArea.count)
        if (new_tray_height < trayWidth){
            trayHeight = new_tray_height
        }
        else{
            trayHeight = trayWidth
        }

        trayIconTabList.height = trayHeight * trayIconTabArea.count
        trayIconShowAll = true
    }

    function trayIconHoverHandler(module_id, index) {
        //console.log(module_id + ": "  + index)
        var tipDisplayHeight
        tipDisplayHeight = Math.abs(trayHeight - trayIconTip.height)/2 + trayHeight * index
        if (trayHeight == trayWidth) {
            tipDisplayHeight = (windowView.height - trayHeight * trayIconTabArea.count)/2 + tipDisplayHeight
        }
        trayIconTip.y = tipDisplayHeight
        trayIconTipText.text = modulesId.moduleLocaleNames[module_id]
        //trayIconTip.opacity = 1.0
        trayIconTip.visible = true
    }

    function resetState(){
        trayIconTabList.currentIndex = -1
        initTrayIcon()
    }

    Timer {
        running: true
        interval: 100
        repeat: false
        onTriggered: {
            initTrayIcon()
        }
    }

    Timer {
        id: timerResetState
        running: false
        interval: 1000 * 10
        repeat: false
        onTriggered: {
            resetState()
        }
    }

    PropertyAnimation {
        id: showingRightBox
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width - 360
        duration: 300
        easing.type: Easing.OutQuad

        onStarted: {
            windowView.show()
        }

        onStopped: {
            displayState = viewState.allShow
        }
    }

    PropertyAnimation {
        id: hidingRoot
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width
        duration: 300
        easing.type: Easing.OutQuad
        
        onStarted: {
            lastDisplayState = displayState
        }

        onStopped: {
            if (lastDisplayState == viewState.allShow){
                if(timerResetState.running){
                    timerResetState.stop()
                }
                timerResetState.restart()
            }else{
                resetState()
            }
            displayState = viewState.allHide
            windowView.hide()
        }
    }

    PropertyAnimation {
        id: showingTrayIconBox
        alwaysRunToEnd: true
        target: windowView
        property: "x"
        to: screenSize.width - 48
        duration: 300
        easing.type: Easing.OutQuad

        onStarted: {
            windowView.show()
        }

        onStopped: {
            displayState = viewState.trayShow
        }
    }
    // animation for root frame

    Rectangle {
        id: frame
        color: Qt.rgba(0, 0, 0, 0)
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 360
        height: root.height
    }

    Window {
        id: trayIconTip
        width: trayIconTipText.width + 52
        height: 44
        x: windowView.x - width
        flags: Qt.Popup
        visible: false
        //opacity: 1

        property alias timeOutHide: hide
        property bool isHover: false

        color: Qt.rgba(0, 0, 0, 0)

        Timer {
            id: hide
            interval: 300
            running: false
            repeat: false
            onTriggered: {
                if (!trayIconTip.isHover){
                    trayIconTip.visible = false
                }
            }
        }

        Behavior on y {
            SmoothedAnimation { duration: 300 }
        }

        RectWithCorner {
            id: trayIconTipArrowRect
            x: 0
            y: 0
            cornerDirection: "right"
            rectWidth: parent.width
            rectHeight: parent.height
            cornerPos: rectHeight/2
            cornerWidth: 16
            cornerHeight: 8
        }

        Text {
            id: trayIconTipText
            anchors.verticalCenter: trayIconTipArrowRect.verticalCenter
            anchors.horizontalCenter: trayIconTipArrowRect.horizontalCenter
            anchors.horizontalCenterOffset: - 4
            font.pixelSize: 13
            color: "white"
        }
    }

    Rectangle {
        id: trayFrame
        width: trayWidth
        anchors.left: frame.left
        anchors.top: frame.top
        anchors.bottom: frame.bottom
        color: defaultBackgroundColor
        //visible: false

        Rectangle {
            color: defaultBackgroundColor
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            ListView {
                property QtObject currentHoverItem

                id: trayIconTabList
                width: parent.width
                anchors.centerIn: parent

                delegate: TabButtonDelegate{width: trayWidth; height: trayHeight}
                model: ListModel {id: trayIconTabArea}
                currentIndex: -1
                onCurrentIndexChanged: {
                    if (currentIndex != -1){
                        showRightBox(currentItem.rightboxId)
                    }
                }
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 3; }
                highlightMoveVelocity: 800
                highlightFollowsCurrentItem: true
                maximumFlickVelocity: 0
                Behavior on height {
                    NumberAnimation { 
                        duration: 300 
                    }
                }
            }
        }
    }

    // separator lines for trayicon and rightbox
    Rectangle {
        width: 1
        anchors.fill: frame
        anchors.leftMargin: trayWidth
        color: "#0b0809"
    }
    Rectangle {
        width: 1
        anchors.fill: frame
        anchors.leftMargin: trayWidth + 1
        color: "#3b3b3d"
    }
    // separator lines for trayicon and rightbox

    Rectangle {
        id: rightBox
        anchors.fill: frame
        anchors.leftMargin: trayWidth + 2
        color: defaultBackgroundColor

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
