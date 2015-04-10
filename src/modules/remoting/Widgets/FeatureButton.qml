/*************************************************************
*File Name: FeatureButton.qml
*Author: Match
*Email: Match.YangWanQing@gmail.com
*Created Time: 2015年04月10日 星期五 13时23分37秒
*Description:
*
*************************************************************/
import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: featureButton

    property alias normalImage: iconImg.normal_image
    property alias hoverImage: iconImg.hover_image
    property alias pressImage: iconImg.press_image
    property alias title: titleText.text
    property alias text: contentText.text
    property var dconstants: DConstants {}

    height: 50
    width: parent.width
    color: dconstants.bgColor

    signal clicked

    DSeparatorHorizontal {
        id:separator1
        anchors.top: parent.top
    }

    DImageButton {
        id: iconImg
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 18
    }

    Text {
        id: titleText
        width: contentWidth
        height: 20
        verticalAlignment: Text.AlignTop
        anchors {top: parent.top;topMargin: 12; left: iconImg.right; leftMargin: 22}
        font.pixelSize: 12
        color: dconstants.fgColor
    }

    Text {
        id: contentText
        lineHeight: 1.2
        width: parent.width -iconImg.width - rightArrow.width - 18 - 22
        height: parent.height - title.height - 12
        verticalAlignment: Text.AlignTop
        anchors {top: titleText.bottom; left: titleText.left}
        font.pixelSize: 10
        wrapMode: Text.Wrap
        color: "#7C7C7C"
    }

    DImageButton {
        id: rightArrow
        anchors {right: parent.right; rightMargin: 14; verticalCenter: parent.verticalCenter}
        normal_image: "qrc:///modules/remoting/images/right-arrow-normal.png"
        hover_image: "qrc:///modules/remoting/images/right-arrow-hover.png"
        press_image: "qrc:///modules/remoting/images/right-arrow-press.png"
        onClicked: featureButton.clicked()
    }
}
