import QtQuick 2.1
import QtQuick.Window 2.0

Item {
    id: root
    property int trayWidth: 48
    property int trayHeight: trayWidth
    property color defaultBackgroundColor: "#252627"
    property bool trayIconShowAll: false

    property int viewHoverPadding: 100

    property int displayState: viewState.allHide

    signal enterMouseArea()
    signal clickOutArea(int mousex, int mousey)

    QtObject { 
        // enumeration for root view state
        id: viewState
        property int allHide: 0
        property int trayShow: 1
        property int rightBoxShow: 2
        property int allShow: 3
    }

    onEnterMouseArea: {
        displayTrayIcon()
    }

    onClickOutArea: {
        if ((root.displayState == viewState.trayShow) && (
            mousex <= screenSize.width - trayWidth)) {
            hideTrayIcon()
        }
        else if ((root.displayState == viewState.allShow) && (
            mousex <= screenSize.width - root.width)) {
            hideTrayIcon()
        }
    }

    function hideTrayIcon(){
        if (!hidingRoot.running){
            hidingRoot.restart()
        }
    }

    function displayTrayIcon(){
        console.log("emit...")
        if (!showingTrayIconBox.running){
            showingTrayIconBox.restart()
        }
    }

    function showRightBox(trayIconId) {
        if (trayIconId == 'shutdown'){
            var d = new Date()
            console.log(d.toLocaleString())
            Qt.quit()
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
        var icon_path_array = modulesId.common_ids()
        var new_tray_height = screenSize.height/(icon_path_array.length+1.0)
        if (new_tray_height < trayWidth){
            trayHeight = new_tray_height
        }
        else{
            trayHeight = trayWidth
        }
        trayIconTabArea.clear()
        for (var i in icon_path_array){
            trayIconTabArea.append({
                'iconId': icon_path_array[i]
            })
        }
        trayIconTabList.height = trayHeight * trayIconTabArea.count
        trayIconShowAll = false
    }

    function expandHideTrayIcon() {
        trayIconTabArea.remove(modulesId.common_ids().length - 2)
        trayIconTabList.currentIndex = -1
        var newIds = modulesId.hide_ids()
        for (var i in newIds){
            var index = trayIconTabArea.count - 2
            trayIconTabArea.insert(index, {'iconId': newIds[i]})
        }
        var new_tray_height = screenSize.height/(trayIconTabArea.count)
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
            tipDisplayHeight = (screenSize.height - trayHeight * trayIconTabArea.count)/2 + tipDisplayHeight
        }
        trayIconTip.y = tipDisplayHeight
        trayIconTipText.text = modulesId.module_names()[module_id]
        trayIconTip.visible = true
    }

    Timer {
        // init something
        running: true
        interval: 100
        repeat: false
        onTriggered: {
            initTrayIcon()
            trayIconTabList.positionViewAtEnd()
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

        /***
        onStarted: {
            windowView.x = 0
        }
        ***/

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
        onStopped: {
            //windowView.x = screenSize.width - 2
            trayIconTabList.currentIndex = -1
            displayState = viewState.allHide
            initTrayIcon()
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

        /**
        onStarted: {
            windowView.x = 0
        }
        **/

        onStopped: {
            displayState = viewState.trayShow
        }
    }
    // animation for root frame

    MouseArea {
        id: fullscreenMouseArea
        anchors.fill: root
        hoverEnabled: true
        onEntered: {
            //console.log("Enter...")
            var min_y = viewHoverPadding
            var max_y = screenSize.height - viewHoverPadding
            if (mouseY > min_y && mouseY < max_y){
                //root.enterMouseArea()
            }
        }
        onClicked: {
            if ((displayState == viewState.allShow && mouseX < root.width - 360)
            || (displayState == viewState.trayShow && mouseX < root.width - 48))
            {
                //root.clickOutArea()
            }
        }
    }

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
        visible: false
        flags: Qt.Popup

        color: Qt.rgba(0, 0, 0, 0)

        RightArrowTip {
            id: trayIconTipArrowRect
            x: 0
            y: 0
            rectWidth: parent.width
            rectHeight: parent.height
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
                        showRightBox(currentItem.trayIconId)
                    }
                }
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 3; }
                highlightMoveVelocity: 800
                highlightFollowsCurrentItem: true
                maximumFlickVelocity: 0
                Behavior on height {
                    NumberAnimation { duration: 300 }
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

        DssLaunchPad {
            id: dssLaunchPad
            //visible: false
        }

        Item {
            id: rightBoxLoaderItem

            property string iconId

            visible: false
            clip: true
            onIconIdChanged: {
                dssLaunchPad.visible = (iconId == '' ? true : false)
                rightBoxLoaderItem.visible = (iconId == '' ? false : true)
                rightBoxLoader.iconId = iconId
                if (iconId){
                    rightBoxLoader.source = '../modules/' + iconId + '/main.qml'
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
                property color childColor: defaultBackgroundColor
                focus: true
                source: ''
                anchors.fill: parent
            }
        }
    }
}
