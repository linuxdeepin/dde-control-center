import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: expandRect

    property alias header: headerRect
    property alias content: content
    property bool expanded: false

    property var dconstants: DConstants {}

    property int separatorPadding: expanded ? 4 : 2

    height: header.height + contentRect.height + separatorPadding
    width: parent.width

    signal contentLoaded

    Column {
        width: parent.width

        Rectangle {
            id: header
            width: parent.width
            height: 28
            clip: true
            color: dconstants.bgColor

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
            height: expanded ? content.height : 0
            clip: true
            color: dconstants.contentBgColor

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

        DSepratorHorizontal { visible: expanded }
    }
}

