import QtQuick 2.1
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

FocusScope {
    id: root
    width: 40
    height: 22
    state: "normal"

    property alias text: text_input.text
    property int textInputRightMargin: 0
    property int textInputLeftMargin: 0
    property alias input: text_input

    property int min: 0
    property int max: 255

    signal accepted
    signal toNext
    signal toPrevious

    function increase(){
        var value = parseInt(text_input.text)
        if(value == max){
            text_input.text = min
        }
        else{
            value += 1
            text_input.text = value
        }
    }

    function decrease(){
        var value = parseInt(text_input.text)
        if(value == min){
            text_input.text = max
        }
        else{
            value -= 1
            text_input.text = value
        }
    }

    Item {
        clip: true
        anchors.fill: parent
        anchors.leftMargin: root.textInputLeftMargin
        anchors.rightMargin: root.textInputRightMargin

        TextInput {
            id: text_input

            focus: true
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignHCenter
            validator: IntValidator{bottom: min; top: max;}
            color: dconstants.fgColor
            font.pixelSize: 14
            selectionColor: "#31536e"
            selectByMouse: true

            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            onActiveFocusChanged: {
                if(activeFocus){
                    selectAll()
                }
                else{
                    deselect()
                }
            }

            onTextChanged: {
                if(text != ""){
                    text = parseInt(text)
                }
            }

            onAccepted: {
                root.accepted()
            }

            Keys.onPressed: {
                // Space ==> 32
                // Tab ==> 16777217
                if (event.text == "."
                    || event.key == 32
                    || event.key == 16777217
                ){
                    toNext()
                }
                // Shift+Tab ==> 16777218
                else if( event.key == 16777218 ){
                    toPrevious()
                }
                // backspace ==> 16777219
                else if(event.key == 16777219){ 
                    if(text_input.length == 0){
                        toPrevious()
                    }
                }
            }

            Keys.onDownPressed:{
                root.decrease()
            }

            Keys.onUpPressed: {
                root.increase()
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    parent.forceActiveFocus()
                }
                onWheel: {
                    if(wheel.angleDelta.y > 0){
                        root.increase()
                    }
                    else{
                        root.decrease()
                    }
                }
            }

        }
    }

}

