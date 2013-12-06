import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Item {
    property string title: "untitled"
    property alias toggled: toggle.checked
    property alias content: content.sourceComponent

    height: header.height + content.height + 4
    width: parent.width

    ColumnLayout {
        width: parent.width
        Rectangle {
            id: header
            width: parent.width
            height: 32
            color: "#303132"
            ColumnLayout {
                width: parent.width
                anchors.verticalCenter: header.verticalCenter
                DSepratorHorizontal {}
                RowLayout {
                    anchors.left:parent.left
                    anchors.leftMargin: 10
                    spacing: parent.width-a.width-toggle.width - anchors.leftMargin
                    Label {
                        id:a
                        text: title
                        color: fgColor
                    }
                    DSwitcherButton {
                        id: toggle
                        onClicked: calcContentHeight()
                    }
                }
                DSepratorHorizontal {}
            }
        }
        Loader {
            id: content
            onLoaded: {
                calcContentHeight()
            }
        }
    }

    function calcContentHeight () {
        content.visible = toggle.checked
        if (toggle.checked) {
            content.height = content.childrenRect.height
        } else {
            content.height = -4
        }
    }
}
