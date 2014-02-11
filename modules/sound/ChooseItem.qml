import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Rectangle {
    width: parent.width
    height: lineHeight
    color: dconstants.contentBgColor

    property var itemObj: obj
            
    Image {
        id: nameImage
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: contentLeftMargin
        source: "images/select.png"
        opacity: parent.ListView.isCurrentItem ? 1 : 0
    }
    
    DssH3 {
        id: nameText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: nameImage.right
        anchors.leftMargin: 6
        
        text: name
        color: parent.ListView.isCurrentItem ? Qt.rgba(0, 144/255, 1, 1.0) :dconstants.fgColor
        font.pixelSize: 12
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.ListView.view.currentIndex = index
        }
    }
}
