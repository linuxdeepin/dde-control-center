import QtQuick 2.1

Item {
    id: root
    property int trayWidth: 48
    property int trayHeight: trayWidth
    property color defaultBackgroundColor: "#252627"
    property bool trayIconShowAll: false

    property int viewHoverPadding: 100

    property int displayState: viewState.allHide

    // animation for root frame
    QtObject { // enumeration for root view state
        id: viewState
        property int allHide: 0
        property int trayShow: 1
        property int rightBoxShow: 2
        property int allShow: 3
    }

    signal enterMouseArea
    signal clickOutArea

    Timer {
        id: displayTimer
        interval: 800
        repeat: false
        onTriggered: {
            if (!showingTrayIconBox.running){
                showingTrayIconBox.restart()
            }
        }
    }

    onEnterMouseArea: {
        if (displayState == viewState.allHide){
            displayTimer.restart()
        }
    }

    onClickOutArea: {
        if (!hidingRoot.running){
            hidingRoot.restart()
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
        trayIconTip.text = module_id
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
        target: frame
        property: "anchors.leftMargin"
        to: root.width - 360
        duration: 300
        easing.type: Easing.OutQuad

        onStarted: {
            windowView.x = 0
        }

        onStopped: {
            displayState = viewState.allShow
        }
    }

    PropertyAnimation {
        id: hidingRoot
        alwaysRunToEnd: true
        target: frame
        property: "anchors.leftMargin"
        to: root.width + 360
        duration: 300
        easing.type: Easing.OutQuad
        onStopped: {
            windowView.x = screenSize.width - 2
            trayIconTabList.currentIndex = -1
            displayState = viewState.allHide
            initTrayIcon()
        }
    }

    PropertyAnimation {
        id: showingTrayIconBox
        alwaysRunToEnd: true
        target: frame
        property: "anchors.leftMargin"
        to: root.width - 48
        duration: 300
        easing.type: Easing.OutQuad

        onStarted: {
            windowView.x = 0
        }
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
            var min_y = viewHoverPadding
            var max_y = screenSize.height - viewHoverPadding
            if (mouseY > min_y && mouseY < max_y){
                root.enterMouseArea()
            }
        }
        onClicked: {
            if ((displayState == viewState.allShow && mouseX < root.width - 360)
            || (displayState == viewState.trayShow && mouseX < root.width - 48))
            {
                root.clickOutArea()
            }
        }
    }

    Rectangle {
        id: frame
        color: Qt.rgba(0, 0, 0, 0)
        anchors.left: parent.left
        anchors.leftMargin: parent.width + 360
        anchors.top: parent.top
        anchors.bottom: parent.bottom
		width: 360
		height: root.height
    }

    Rectangle {
        id: trayIconTip
        width: trayIconTipImageLeft.width + trayIconTipImageMiddle.width + trayIconTipImageRight.width
        height: 30
        //anchors.top: frame.top
        anchors.right: frame.left
        //anchors.rightMargin: 5
        color: frame.color
        visible: false

        property string text

        Image {
            id: trayIconTipImageLeft
            anchors.top: parent.top
            anchors.left: parent.left
            source: "images/tooltips_left.png"
        }
        
        Image {
            id: trayIconTipImageMiddle
            anchors.top: parent.top
            anchors.left: trayIconTipImageLeft.right
            source: "images/tooltips_middle.png"
            width: trayIconTipText.width + 4
        }
        Image {
            id: trayIconTipImageRight
            anchors.top: parent.top
            anchors.left: trayIconTipImageMiddle.right
            source: "images/tooltips_right.png"
        }

        Text {
            id: trayIconTipText
            anchors.centerIn: trayIconTipImageMiddle
            text: trayIconTip.text
            font.pixelSize: 12
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
            anchors.bottomMargin: bottomShutdownButton.visible ? bottomShutdownButton.height : 0
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

        /***
        Rectangle {
            id: topMoreButton
            width: parent.width
            height: 30
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: defaultBackgroundColor
            visible: false

            ImageButton {
                nomralImage: "images/more.png"
                hoverImage: nomralImage
                pressImage: nomralImage

                anchors.fill: parent
                color: defaultBackgroundColor

                onClicked: {
                    topMoreButton.visible = false
                    trayIconTabList.anchors.topMargin = 0
                    trayIconTabList.anchors.bottomMargin = bottomMoreButton.height + bottomShutdownButton.height
                    bottomMoreButton.visible = true
                    trayIconTabList.positionViewAtBeginning()
                    bottomShutdownButtonLine.visible = false
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width
                height: 1
                color: Qt.rgba(255, 255, 255, 0.1)
                //visible: false
            }
        }

        Rectangle {
            id: bottomMoreButton
            width: parent.width
            height: 30
            anchors.bottom: bottomShutdownButton.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: defaultBackgroundColor
            visible: false

            ImageButton {
                nomralImage: "images/more.png"
                hoverImage: nomralImage
                pressImage: nomralImage

                anchors.fill: parent
                color: defaultBackgroundColor

                onClicked: {
                    topMoreButton.visible = true
                    trayIconTabList.anchors.topMargin = topMoreButton.height
                    trayIconTabList.anchors.bottomMargin = bottomShutdownButton.height
                    bottomMoreButton.visible = false
                    trayIconTabList.positionViewAtEnd()
                    bottomShutdownButtonLine.visible = true
                }
            }

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                width: parent.width
                height: 1
                color: Qt.rgba(255, 255, 255, 0.1)
                //visible: false
            }
        }
        ***/

        Rectangle {
            id: bottomShutdownButton
            color: defaultBackgroundColor
            width: parent.width
            height: trayHeight
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            visible: false

            ImageButton {
                nomralImage: "trayicon_images/shutdown_normal.png"
                hoverImage: "trayicon_images/shutdown_hover.png"
                pressImage: "trayicon_images/shutdown_press.png"

                anchors.fill: parent
                color: defaultBackgroundColor

                onClicked: {
                    Qt.quit()
                }

                onHoverChanged: {
                    if (hover){
                        trayIconTip.anchors.right = frame.left
                        trayIconTip.anchors.rightMargin = 5
                        trayIconTip.anchors.bottom = frame.bottom
                        trayIconTip.anchors.bottomMargin = Math.abs(bottomShutdownButton.height - trayIconTip.height)/2
                        trayIconTip.text = "Shut Down"
                        trayIconTip.visible = true
                    }
                    else {
                        trayIconTip.visible = false
                    }
                }
            }

            Rectangle {
                id: bottomShutdownButtonLine
                anchors.top: parent.top
                anchors.left: parent.left
                width: parent.width
                height: 1
                color: Qt.rgba(255, 255, 255, 0.1)
                visible: false
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
