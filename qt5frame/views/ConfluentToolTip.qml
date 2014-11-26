/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <match.yangwanqing@gmail.com>
**  Maintainer: Wanqing Yang <match.yangwanqing@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.1
import QtQml 2.2
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

Item {
    id:rootItem
    height: rectangleHeight + arrowHeight

    property alias middleX: anchorsItem.x
    property int arrowHeight: 10
    property int arrowWidth: 15
    property int rectangleHeight: 25
    property int animationDuration: 300
    property int autoHideInterval: -1
    property int delayShowInterval: -1

    state: "hideAll"

    function showToolTip(x,y,mX,tips){
        rootItem.x = x
        rootItem.y = y
        rootItem.middleX = mX
        tipText.text = tips.replace(/[\r\n]/g,"  ")

        delayShowTimer.stop()
        delayShowTimer.start()
    }

    function hideToolTip(){
        delayShowTimer.stop()
        rootItem.state = "hideTip"
    }

    onYChanged: {
        if (autoHideInterval > 0){
            autoHideTimer.stop()
            autoHideTimer.start()
        }

    }
    onMiddleXChanged: {
        if (autoHideInterval > 0){
            autoHideTimer.stop()
            autoHideTimer.start()
        }
    }

    Behavior on middleX {
        SmoothedAnimation { duration: animationDuration }
    }
    Behavior on y {
        SmoothedAnimation { duration: animationDuration }
    }

    Timer {
        id:autoHideTimer
        repeat: false
        running: false
        interval:autoHideInterval
        onTriggered: hideToolTip()
    }

    Timer {
        id:delayShowTimer
        repeat: false
        running: false
        interval: delayShowInterval > 0 ? delayShowInterval : 0
        onTriggered: {
            rootItem.state = "showTip"

            if (autoHideInterval > 0){
                autoHideTimer.stop()
                autoHideTimer.start()
            }
        }
    }

    Image {
        id:leftCircleRec
        width: 0
        height: 0
        x: leftImage.x <= width / 2 ? 0 : leftImage.x - width / 2
        anchors.bottom: arrowTriangle.top
        source: "qrc:/views/images/circle.png"
    }
    Image {
        id: rightCircleRec
        width: 0
        height: 0
        x: rightImage.width >= rootItem.width - width / 2 ? rootItem.width - width / 2 :  middleX + rightImage.width - width / 2
        anchors.bottom: arrowTriangle.top
        source: "qrc:/views/images/circle.png"
    }

    Item {
        id:anchorsItem
        width: 0
        height: parent.height
    }

    Image {
        id:leftImage
        height: rectangleHeight
        width: 0
        fillMode: Image.TileHorizontally
        anchors.right: anchorsItem.left
        anchors.bottom: arrowTriangle.top
        anchors.bottomMargin: -1
        source: "qrc:/views/images/confluent_back.png"

        Rectangle{
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: parent.width - arrowTriangle.width / 2
            height: 1
            color: Qt.rgba(255,255,255,0.10)
        }
    }
    InnerShadow {
        anchors.fill: leftImage
        radius: 1.5
        samples: 16
        horizontalOffset: 0
        verticalOffset: 2
        color: "#1a1a1a"
        source: leftImage
    }

    Image {
        id:rightImage
        height: rectangleHeight
        width: 0
        fillMode: Image.TileHorizontally
        anchors.left: anchorsItem.right
        anchors.bottom: arrowTriangle.top
        anchors.bottomMargin: -1
        source: "qrc:/views/images/confluent_back.png"

        Rectangle{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: parent.width - arrowTriangle.width / 2
            height: 1
            color: Qt.rgba(255,255,255,0.10)
        }
    }
    InnerShadow {
        anchors.fill: rightImage
        radius: 1.5
        samples: 16
        horizontalOffset: 0
        verticalOffset: 2
        color: "#1a1a1a"
        source: rightImage
    }

    //will be place by triangle
    Image {
        id:arrowTriangle
        height: arrowHeight
        width: 0
        anchors.bottom: parent.bottom
        x: middleX - width / 2
        source: "qrc:/views/images/triangle.png"
    }

    Text {
        id:tipText
        color: dconstants.tuhaoColor
        font.pixelSize: 12
        opacity: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: leftImage.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    states: [
        State {
            name: "showTip"
            PropertyChanges {target: leftImage; width: middleX;height:rectangleHeight; opacity:1}
            PropertyChanges {target: rightImage; width: rootItem.width - middleX;height:rectangleHeight; opacity: 1}
//            PropertyChanges {target: leftCircleRec; width: rectangleHeight; height: rectangleHeight}
//            PropertyChanges {target: rightCircleRec; width: rectangleHeight; height: rectangleHeight}
            PropertyChanges {target: arrowTriangle; width: arrowWidth}
            PropertyChanges {target: tipText; opacity: 1}
        },
        State {
            name: "hideTip"
            PropertyChanges {target: leftImage; width: 0;height:rectangleHeight / 2; opacity: 0.5}
            PropertyChanges {target: rightImage; width: 0;height:rectangleHeight / 2; opacity: 0.5}
//            PropertyChanges {target: leftCircleRec; width: 0; height: 0}
//            PropertyChanges {target: rightCircleRec; width: 0; height: 0}
            PropertyChanges {target: arrowTriangle; width: 0}
            PropertyChanges {target: tipText; opacity: 0}
        }

    ]

    transitions: [
//        Transition {
//            from: "showTip"
//            to: "hideTip"
//            SequentialAnimation {
//                NumberAnimation {target: tipText; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
//                ParallelAnimation {
//                    NumberAnimation {target: leftImage; property: "width"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: leftImage; property: "height"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: leftImage; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: rightImage; property: "width"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: rightImage; property: "height"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: rightImage; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
//                    NumberAnimation {target: arrowTriangle; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
//                }

////                ParallelAnimation {
////                    NumberAnimation {target: leftCircleRec; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
////                    NumberAnimation {target: leftCircleRec; property: "height"; duration: animationDuration;easing.type: Easing.OutCubic}
////                    NumberAnimation {target: rightCircleRec; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
////                    NumberAnimation {target: rightCircleRec; property: "height"; duration: animationDuration;easing.type: Easing.OutCubic}
////                    NumberAnimation {target: arrowTriangle; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
////                }
//            }
//        },
        Transition {
            from: "hideTip"
            to: "showTip"
            SequentialAnimation {
//                ParallelAnimation {
//                    NumberAnimation {target: leftCircleRec; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
//                    NumberAnimation {target: leftCircleRec; property: "height"; duration: animationDuration;easing.type: Easing.OutCubic}
//                    NumberAnimation {target: rightCircleRec; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
//                    NumberAnimation {target: rightCircleRec; property: "height"; duration: animationDuration;easing.type: Easing.OutCubic}
//                    NumberAnimation {target: arrowTriangle; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
//                }

                ParallelAnimation {
                    NumberAnimation {target: leftImage; property: "width"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: leftImage; property: "height"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: leftImage; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: rightImage; property: "width"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: rightImage; property: "height"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: rightImage; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
                    NumberAnimation {target: arrowTriangle; property: "width"; duration: animationDuration;easing.type: Easing.OutCubic}
                }
                NumberAnimation {target: tipText; property: "opacity"; duration: animationDuration; easing.type: Easing.OutCubic}
            }
        }

    ]


}
