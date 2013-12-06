import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: header

    property alias title: titleArea
    property alias button: actionArea
    property int leftMargin: 18
    property int rightMargin: 5
    property var dconstants: DConstants {}

    height: 30
    width: parent.width
    color: dconstants.bgColor

    signal titleLoaded
    signal buttonLoaded

    Loader {
        id: titleArea
        anchors.left: parent.left
        anchors.leftMargin: leftMargin
        anchors.verticalCenter: parent.verticalCenter
        //height: childrenRect.height
        onLoaded: { header.titleLoaded() }
    }

    Loader {
        id: actionArea
        anchors.right: parent.right
        anchors.rightMargin: rightMargin
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -1
        clip: true
        onLoaded: { header.buttonLoaded() }
    }
}
