import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../../shared"

Item {
    id: notManagedArea
    width: 310
    height: Math.max(tipContent.height + 50, 90)
    clip: true

    property string titleText: "Wireless Network"
    property string contentText: "The hardware switch of wireless network card is off, please switch on as necessary."

    DBaseLine {
        id: title
        leftLoader.sourceComponent: DssH2 {
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            text: notManagedArea.titleText
        }
        rightLoader.sourceComponent: DssH2{
            anchors.verticalCenter: parent.verticalCenter
            text: dsTr("not managed")
            color: errorColor
        }
    }

    DSeparatorHorizontal {
        anchors.top: title.bottom
    }

    Item {
        width: parent.width
        height: notManagedArea.height - 26
        anchors.bottom: parent.bottom

        ArrowRect {
            id: arrowRectBackground
            anchors.fill: parent
            fillStyle: dconstants.contentBgColor
            stroke: false
            radius: 0
            lineWidth: 0
            arrowPosition: 0.85
        }

        DssH2 {
            id: tipContent
            font.pixelSize: 13
            text: notManagedArea.contentText
            width: parent.width - tipImage.width - 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 4
            anchors.left: parent.left
            anchors.leftMargin: 15
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
        }

        Image {
            id: tipImage
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 3
            anchors.right: parent.right
            anchors.rightMargin: 10
            source: "../images/ico_warning.png"
        }
    }
}
