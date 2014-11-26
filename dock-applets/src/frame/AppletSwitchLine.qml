import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id:rootItem
    signal clicked(string switchTitle, bool switchState)
    property string switchId:applet_id
    property string switchTitle:applet_name
    property string switchIcon:applet_icon
    property bool switchState:applet_visible

    visible: setting_enable

    height: setting_enable ? 38 : 0
    width: parent.width

    DIcon {
        id:itemIcon
        icon: switchIcon
        theme: "Deepin"
        width: 16
        height: 16
        anchors {left: parent.left; leftMargin: 15; verticalCenter: parent.verticalCenter}
    }

    Text {
        id:itemText
        height: parent.heights
        width: parent.width / 2
        elide: Text.ElideRight
        color: "#ffffff"
        font.pixelSize: 13
        text: switchTitle
        verticalAlignment: Text.AlignLeft
        anchors.left: itemIcon.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
    }

    DSwitchButton {
        checked: switchState
        onClicked:rootItem.clicked(switchId,checked)
        anchors {right: parent.right; rightMargin: 5; verticalCenter: parent.verticalCenter}
    }
}
