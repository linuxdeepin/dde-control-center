import QtQuick 2.1
import Deepin.Widgets 1.0

Item{
    height: parent.height
    width: childrenRect.width

    Rectangle{
        width: 2
        height: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        color: DConstants.fgColor
    }
}
