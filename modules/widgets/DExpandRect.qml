import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: expandRect

    property alias header: headerRect.sourceComponent
    property alias content: content.sourceComponent

    height: header.height + contentRect.height
    width: parent.width
    property bool _expand_: false

    signal contentLoaded

    function toggle() {
        _expand_ = !_expand_
        contentRect.height = _expand_ ? content.height : 0
    }

    Column {
        width: parent.width

        Rectangle {
            id: header
            width: parent.width
            height: 28
            clip: true
            color: bgColor

            Loader {
                id: headerRect
                width: header.width
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        DSepratorHorizontal {}

        Rectangle {
            id: contentRect
            width: parent.width
            height: 0
            clip: true
            color: contentBgColor

            Loader { 
                width: header.width
                id: content 
                onLoaded: { 
                    content.height = content.childrenRect.height 
                    expandRect.contentLoaded()
                }
                anchors.top: parent.top
                anchors.left: parent.left
            }

            Behavior on height {
                SmoothedAnimation { duration: 200 }
            }

        }

    }
}

