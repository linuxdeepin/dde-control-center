import QtQuick 2.1

Item {
    id: root
    property int trayWidth: 48
    property color defaultBackgroundColor: "#252627"

    property int displayState: viewState.hideAll

    // animation for root frame
    QtObject { // enumeration for root view state
        id: viewState
        property int hideAll: 0
        property int showTray: 1
        property int showDss: 2
        property int showAll: 3
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
        if (displayState == viewState.hideAll){
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
        }
        else{
            if (!showingRightBox.running && !showingTrayIconBox.running){
                showingRightBox.restart()
            }
            if (trayIconId == 'dss'){
                rightBoxLoaderItem.iconId = ''
            }
            else{
                rightBoxLoaderItem.iconId = trayIconId
            }
        }
    }

    function initTrayIcon() {
        var icon_path_array = modulesId
        for (var i in icon_path_array){
            trayIconTabArea.append({'iconId': icon_path_array[i]})
        }
        //trayIconOutArea.height = icon_path_array.length * trayWidth
    }

    Timer {
        // init something
        running: true
        interval: 100
        repeat: false
        onTriggered: {
            initTrayIcon()
            trayIconTabList.positionViewAtEnd()
            trayIconTabList.cancelFlick()
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
            displayState = viewState.showAll
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
            displayState = viewState.hideAll
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
            displayState = viewState.showTray
        }
    }
    // animation for root frame

    MouseArea {
        id: fullscreenMouseArea
        anchors.fill: root
        hoverEnabled: true
        onEntered: {
            root.enterMouseArea()
        }
        onClicked: {
            if ((displayState == viewState.showAll && mouseX < root.width - 360)
            || (displayState == viewState.showTray && mouseX < root.width - 48))
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
        width: trayIconTipText.width + 10
        height: 30
        color: defaultBackgroundColor

        property string text

        Text {
            id: trayIconTipText
            anchors.centerIn: parent
            text: trayIconTip.text
            font.pixelSize: 10
            styleColor: "white"
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

        ListView {
            property QtObject currentHoverItem

            id: trayIconTabList
            width: parent.width
            height: parent.height - topMoreButton.height - bottomShutdownButton.height
            anchors.top: parent.top
            anchors.topMargin: topMoreButton.height
            anchors.bottom: parent.bottom
            anchors.bottomMargin: bottomShutdownButton.height
            anchors.left: parent.left
            delegate: TabButtonDelegate{width: trayWidth; height: trayWidth}
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

            /***
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPositionChanged: {
                    var item = trayIconTabList.itemAt(mouseX, mouseY)
                    if (item != trayIconTabList.currentHoverItem) {
                        trayIconTabList.currentHoverItem = item
                        console.log(item)
                    }
                }
            }
            ***/
        }

        Rectangle {
            id: topMoreButton
            width: parent.width
            height: 30
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: defaultBackgroundColor

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

        Rectangle {
            id: bottomShutdownButton
            color: defaultBackgroundColor
            width: parent.width
            height: parent.width
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

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
                //visible: false
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
