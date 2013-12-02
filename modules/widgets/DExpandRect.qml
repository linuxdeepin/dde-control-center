import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: expandRect
    property alias title: titleRect.sourceComponent
    property alias content: content.sourceComponent

    height: header.height + contentRect.height
    width: parent.width

    property bool expand: false
    function toggle() {
        expand = !expand
        contentRect.height = expand ? content.height : 0
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
                id: titleRect
		width: header.width
                anchors.verticalCenter: parent.verticalCenter
            }
        }

	DSepratorHorizontal { id: bottomSep;visible: expand }

        Rectangle {
            id: contentRect
            width: parent.width
            height: 0
            clip: true
            color: contentBgColor

            Loader { 
		width: header.width
                id: content 
                onLoaded: { content.height = content.childrenRect.height }
                anchors.top: parent.top
                anchors.left: parent.left
            }

            Behavior on height {
                NumberAnimation { duration: 200 }
            }

        }

    }
}

