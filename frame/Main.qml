import QtQuick 2.1

Item {
    signal testDbus (string name)
    property bool inInteractiveArea: false
    property int trayWidth: 50

    Rectangle {
        id: trayFrame
        width: trayWidth
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
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
                delegate: TabButton{width: trayWidth; height: trayWidth}
                model: ListModel {id: trayIconTabArea}
                anchors.fill: parent
            }

            Timer {
                id: loadIcon
                interval: 10; running: true; repeat: false
                onTriggered: {
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
                        trayIconTabArea.append({'trayIconImage': icon_path_array[path] + '_normal.png'})
                    }
                    trayIconOutArea.height = icon_path_array.length * trayWidth
                } 
            }
        }
    }

    LauncherList {
        id: dssList
        anchors.fill: parent
        anchors.leftMargin: trayWidth
        Component.onCompleted: {
            addModule("系统更新", Qt.resolvedUrl("../modules/update/Main.qml"), "update");
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
            addModule("系统信息", Qt.resolvedUrl("../modules/system_info/main.qml"), "system_information");
        }
    }
}
