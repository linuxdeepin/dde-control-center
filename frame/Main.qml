import QtQuick 2.0

Item {
    height: 600
    width: 460
    LauncherList {
        id: ll
        anchors.fill: parent
        Component.onCompleted: {
            addModule("显示",  Qt.resolvedUrl("../modules/basics/color-animation.qml"), "display");
            addModule("桌面",  Qt.resolvedUrl("../modules/basics/property-animation.qml"), "desktop");
            addModule("个性化", Qt.resolvedUrl("../modules/behaviors/behavior-example.qml"), "individuation");
            addModule("声音", Qt.resolvedUrl("../modules/behaviors/wigglytext.qml"), "sound");
            addModule("日期和时间", Qt.resolvedUrl("../modules/behaviors/tvtennis.qml"), "date_time");
            addModule("电源", Qt.resolvedUrl("../modules/easing/easing.qml"), "power");
            addModule("键盘", Qt.resolvedUrl("../modules/states/states.qml"), "keyboard");
            addModule("账户", Qt.resolvedUrl("../modules/states/transitions.qml"), "account");
            addModule("网络", Qt.resolvedUrl("../modules/pathanimation/pathanimation.qml"), "network");
            addModule("蓝牙", Qt.resolvedUrl("../modules/pathinterpolator/pathinterpolator.qml"), "bluetooth");
        }
    }
}
