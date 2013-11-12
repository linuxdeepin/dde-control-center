import QtQuick 2.1

Item {
    id: root
    signal testDbus (string name)
    property bool inInteractiveArea: false
    property int trayWidth: 50
    property int trayIconSelectIndex: -1

    function showRightBox(index) {
        //rightBox.visible = true
        //windowView.x = screenSize.width - 360
        if (index == 7){
            if (!hidingRightBox.running && !showingRightBox.running){
                hidingRightBox.restart()
            }
        }
        else{
            if (!showingRightBox.running && !hidingRightBox.running){
                showingRightBox.restart()
            }
        }
    }

    Rectangle {
        id: frame
		opacity: 0
        color: Qt.rgba(0, 0, 0, 0.3)
        anchors.centerIn: parent
		width: root.width
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
                width: parent.width
                anchors.fill: parent
                model: ListModel {id: trayIconTabArea}
                delegate: TabButtonDelegate{width: trayWidth; height: trayWidth}
                currentIndex: root.trayIconSelectIndex
                onCurrentIndexChanged: {
                    root.trayIconSelectIndex = currentIndex
                    showRightBox(currentIndex)
                }
                highlight: Rectangle { color: Qt.rgba(255, 255, 255, 0.1); radius: 3 }
                highlightMoveVelocity: 800
            }

            Component.onCompleted: {
                var icon_path_array = [
                    "trayicon_images/bluetooth",
                    "trayicon_images/dss",
                    "trayicon_images/notice",
                    "trayicon_images/power",
                    "trayicon_images/shutdown",
                    "trayicon_images/sound",
                    "trayicon_images/usb",
                    "trayicon_images/wifi",
                ]
                for (var path in icon_path_array){
                    trayIconTabArea.append({'trayIconImage': icon_path_array[path]})
                }
                trayIconOutArea.height = icon_path_array.length * trayWidth
            }
        }
    }

        
    PropertyAnimation {
        id: showingRightBox
        target: windowView
        property: "x"
        to: screenSize.width -360
        duration: 300
        easing.type: Easing.OutQuad
    }

    PropertyAnimation {
        id: hidingRightBox
        target: windowView
        property: "x"
        to: screenSize.width - 48
        duration: 300
        easing.type: Easing.OutQuad
    }

    Rectangle {
        id: rightBox
        anchors.fill: frame
        anchors.leftMargin: trayWidth

        DssLaunchPad {
            id: dssLaunchPad
            anchors.fill: parent
            Component.onCompleted: {
                addModule(qsTr("System Update"), Qt.resolvedUrl("../modules/system_update/Main.qml"), "update");
                //addModule("显示",  Qt.resolvedUrl("../modules/basics/color-animation.qml"), "display");
                //addModule("桌面",  Qt.resolvedUrl("../modules/basics/property-animation.qml"), "desktop");
                //addModule("个性化", Qt.resolvedUrl("../modules/behaviors/behavior-example.qml"), "individuation");
                //addModule("声音", Qt.resolvedUrl("../modules/behaviors/wigglytext.qml"), "sound");
                //addModule("日期和时间", Qt.resolvedUrl("../modules/behaviors/tvtennis.qml"), "date_time");
                //addModule("电源", Qt.resolvedUrl("../modules/easing/easing.qml"), "power");
                //addModule("键盘", Qt.resolvedUrl("../modules/states/states.qml"), "keyboard");
                //addModule("账户", Qt.resolvedUrl("../modules/states/transitions.qml"), "account");
                //addModule("网络", Qt.resolvedUrl("../modules/pathanimation/pathanimation.qml"), "network");
                //addModule("蓝牙", Qt.resolvedUrl("../modules/pathinterpolator/pathinterpolator.qml"), "bluetooth");
                addModule(qsTr("System Information"), Qt.resolvedUrl("../modules/system_info_bak/main.qml"), "system_information");
            }
        }
    }
}
