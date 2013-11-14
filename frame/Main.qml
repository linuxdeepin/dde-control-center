import QtQuick 2.1

Item {
    id: root
    property bool inInteractiveArea: false
    property int trayWidth: 50
    property int trayIconSelectIndex: -1

    property int displayState: viewState.hideAll

    QtObject { //enumeration
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
        id: trayFrame
        width: trayWidth
        anchors.left: frame.left
        anchors.top: frame.top
        anchors.bottom: frame.bottom
        color: "#252627"
        //visible: false

        Rectangle {
            width: 1
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: "#0b0809"
        }
        Rectangle {
            width: 1
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: "#3b3b3d"
        }

        Rectangle {
            id: trayIconOutArea
            width: parent.width
            color: "#00FFFFFF"
            anchors.centerIn: parent

            ListView {
                id: trayIconTabList
                width: parent.width
                anchors.fill: parent
                model: ListModel {id: trayIconTabArea}
                delegate: TabButtonDelegate{width: trayWidth; height: trayWidth}
                currentIndex: trayIconSelectIndex
                onCurrentIndexChanged: {
                    if (currentIndex != -1){
                        showRightBox(currentItem.trayIconId)
                    }
                }
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 3 }
                highlightMoveVelocity: 800
            }

            Component.onCompleted: {
                var icon_path_array = [
                    "notice",
                    "wifi",
                    "sound",
                    "usb",
                    "bluetooth",
                    "power",
                    "dss",
                    "shutdown",
                ]
                for (var i in icon_path_array){
                    trayIconTabArea.append({'iconId': icon_path_array[i]})
                }
                trayIconOutArea.height = icon_path_array.length * trayWidth
            }
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

    Rectangle {
        id: rightBox
        anchors.fill: frame
        anchors.leftMargin: trayWidth
        color: "#252627"

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
                    rightBoxLoader.source = 'trayicon_modules/' + iconId + '/main.qml'
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
                property color childColor: "#252627"
                focus: true
                source: ''
                anchors.fill: parent
            }
        }
    }
}
