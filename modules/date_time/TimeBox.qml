import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: timeBox
    width: parent.width
    height: 118
    color: "#1a1b1b"

    property bool use24Hour: false
    property bool editable: true

    function showTip(){
        dayText.opacity = 0
        timeChangeTip.opacity = 1
        if(!timeoutHideTimeChangeTip.running){
            timeoutHideTimeChangeTip.start()
        }
    }

    function showTimeEdit(){
        showTimeEditAnimation.start()
        timeEdit.init()
        timeBox.height += buttonArea.height
    }

    function showTimeNormal(){
        showTimeNormalAnimation.start()
        timeBox.height -= buttonArea.height
    }

    Behavior on height{
        PropertyAnimation { duration: 200 }
    }

    SequentialAnimation{
        id: showTimeEditAnimation
        PropertyAnimation{ target: normalTimeBox; property: "opacity"; to: 0; duration: 200 }
        PropertyAnimation{ target: editTimeBox; property: "opacity"; to: 1; duration: 200 }
    }

    SequentialAnimation{
        id: showTimeNormalAnimation
        PropertyAnimation{ target: editTimeBox; property: "opacity"; to: 0; duration: 200 }
        PropertyAnimation{ target: normalTimeBox; property: "opacity"; to: 1; duration: 200 }
    }

    Timer{
        id: timeoutHideTimeChangeTip
        repeat: false
        running: false
        interval: 3000
        onTriggered: {
            dayText.opacity = 1
            timeChangeTip.opacity = 0
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            if(timeBox.editable){
                showTip()
            }
        }
    }

    Item {
        id: normalTimeBox
        width: parent.width
        height: childrenRect.height
        anchors.top: parent.top
        anchors.topMargin: 24
        visible: opacity != 0
        opacity: 1

        TimeNormal {
            id: dynamicTime
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            use24Hour: timeBox.use24Hour

            onDoubleClicked: {
                if(timeBox.editable){
                    showTimeEdit()
                }
            }
        }

        DLabel {
            id: amPmText
            anchors.left: dynamicTime.right
            anchors.bottom: dynamicTime.bottom
            anchors.bottomMargin: -2
            color: "#666"

            font.pixelSize: 14
            font.family: fixedFont.name
            font.bold: true
            visible: opacity != 0
            text: globalDate.getHours() < 12 ? "AM" : "PM"
            opacity: timeBox.use24Hour ? 0 : 1

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

        DLabel {
            id: dayText
            anchors.top: dynamicTime.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dynamicTime.horizontalCenter

            font.pixelSize: 12
            text: globalDate.toLocaleDateString(locale)
            visible: opacity != 0

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

        DLabel {
            id: timeChangeTip
            anchors.top: dynamicTime.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dynamicTime.horizontalCenter
            font.pixelSize: 12
            color: "#F48914"
            text: dsTr("双击数字修改时间")
            visible: opacity != 0
            opacity: 0

            Behavior on opacity{
                PropertyAnimation { duration: 200 }
            }
        }

    }

    Item {
        id: editTimeBox
        anchors.fill: parent
        visible: opacity != 0
        opacity: 0

        TimeEdit{
            id: timeEdit
            onAccepted: {
                gDate.SetTime(timeEdit.currentTime)
                showTimeNormal()
            }
            height: parent.height - buttonArea.height
        }

        Row{
            id: buttonArea
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: childrenRect.width
            height: childrenRect.height
            spacing: 10

            DTextAction {
                text: dsTr("Change")
                onClicked: {
                    gDate.SetTime(timeEdit.currentTime)
                    timeBox.showTimeNormal()
                }
            }

            Rectangle {
                width: 1
                height: cancelButton.height - 10
                anchors.verticalCenter: cancelButton.verticalCenter
                color: dconstants.fgColor
            }

            DTextAction {
                id: cancelButton
                text: dsTr("Cancel")
                onClicked: {
                    timeBox.showTimeNormal()
                }
            }
        }
    }
}
