import QtQuick 2.1
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

Item {
    id: ipv4Input
    width: 176
    height: 22
    clip: true

    property bool isError: false
    property int inputItemHeight: height - 4
    property int inputItemWidth: (width - ipDot.width * 3 - 4)/4

    property var dconstants: DConstants {}

    property bool isFocus: ipInput1.activeFocus || ipInput2.activeFocus || ipInput3.activeFocus || ipInput4.activeFocus

    signal accepted
    signal toNext
    signal toPrevious
    signal textChanged

    function getValue(){
        var t1 = ipInput1.text
        var t2 = ipInput2.text
        var t3 = ipInput3.text
        var t4 = ipInput4.text
        if(t1 == "" && t2 == "" && t3 == "" && t4 == "" ){
            return ""
        }
        else{
            return "%1.%2.%3.%4".arg(t1).arg(t2).arg(t3).arg(t4)
        }
        // TODO remove
        // if(t1 != "" && t2 != "" && t3 != "" && t4 != "" ){
        //     return "%1.%2.%3.%4".arg(t1).arg(t2).arg(t3).arg(t4)
        // }
        // else{
        //     return ""
        // }
    }

    function setValue(ipAddr){
        var valueList = ipAddr.split(".")
        ipInput1.text = valueList[0]
        ipInput2.text = valueList[1]
        ipInput3.text = valueList[2]
        ipInput4.text = valueList[3]
    }

    function getFocus(){
        ipInput1.forceActiveFocus()
    }

    Rectangle {
        id: text_input_box

        width: parent.width
        height: parent.height
        clip: true
        color: "#171717"
        radius: 3
    }

    DropShadow {
        anchors.fill: text_input_box
        radius: 1
        samples: 10
        horizontalOffset: 0
        verticalOffset: 1
        color: Qt.rgba(1, 1, 1, 0.15)
        source: text_input_box
    }

    InnerShadow {
        anchors.fill: text_input_box
        radius: 1
        samples: 10
        horizontalOffset: 0
        verticalOffset: radius
        color: "black"
        source: text_input_box
    }

    Row {
        id: content
        anchors.centerIn: parent

        Ipv4InputItem{
            id: ipInput1
            width: inputItemWidth
            height: inputItemHeight
            onToNext: {
                ipInput2.forceActiveFocus()
                if(resetPosition){
                    ipInput2.setPositionToStart()
                }
            }
            onToPrevious: {
                ipv4Input.toPrevious()
            }
            onToStart: {
                input.cursorPosition = 0
            }
            onToEnd: {
                ipInput4.forceActiveFocus()
                ipInput4.setPositionToEnd()
            }
            onTextChanged: ipv4Input.textChanged()
        }

        Ipv4Dot{id: ipDot}

        Ipv4InputItem{
            id: ipInput2
            width: inputItemWidth
            height: inputItemHeight
            onToNext: {
                ipInput3.forceActiveFocus()
                if(resetPosition){
                    ipInput3.setPositionToStart()
                }
            }
            onToPrevious: {
                ipInput1.forceActiveFocus()
                if(resetPosition){
                    ipInput1.setPositionToEnd()
                }
            }
            onToStart: {
                ipInput1.forceActiveFocus()
                ipInput1.input.cursorPosition = 0
            }
            onToEnd: {
                ipInput4.forceActiveFocus()
                ipInput4.setPositionToEnd()
            }
            onTextChanged: ipv4Input.textChanged()
        }

        Ipv4Dot{}

        Ipv4InputItem{
            id: ipInput3
            width: inputItemWidth
            height: inputItemHeight
            onToNext: {
                ipInput4.forceActiveFocus()
                if(resetPosition){
                    ipInput4.setPositionToStart()
                }
            }
            onToPrevious: {
                ipInput2.forceActiveFocus()
                if(resetPosition){
                    ipInput2.setPositionToEnd()
                }
            }
            onToStart: {
                ipInput1.forceActiveFocus()
                ipInput1.input.cursorPosition = 0
            }
            onToEnd: {
                ipInput4.forceActiveFocus()
                ipInput4.setPositionToEnd()
            }
            onTextChanged: ipv4Input.textChanged()
        }

        Ipv4Dot{}

        Ipv4InputItem{
            id: ipInput4
            width: inputItemWidth
            height: inputItemHeight
            onToNext: {
                ipv4Input.toNext()
            }
            onToPrevious: {
                ipInput3.forceActiveFocus()
                if(resetPosition){
                    ipInput3.setPositionToEnd()
                }
            }

            onAccepted: ipv4Input.accepted()

            onToStart: {
                ipInput1.forceActiveFocus()
                ipInput1.input.cursorPosition = 0
            }
            onToEnd: {
                ipInput4.setPositionToEnd()
            }
            onTextChanged: ipv4Input.textChanged()
        }
    }
    Rectangle {
        id: text_input_box_border
        radius: 3
        color: "transparent"
        border.color: isError ? "#F48914" : "black"

        anchors.fill:text_input_box
    }

}
