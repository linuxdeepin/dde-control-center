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
    signal toNext(bool resetPosition)
    signal toPrevious(bool resetPosition)

    signal toStart
    signal toEnd

    function setPositionToStart(){
        text_input.cursorPosition = 0
    }

    function setPositionToEnd(){
        if(text_input.length != 0){
            text_input.cursorPosition = text_input.length
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
                    toNext(false)
                }
                // Shift+Tab ==> 16777218
                else if( event.key == 16777218 ){
                    toPrevious(false)
                }
                // backspace ==> 16777219
                else if(event.key == 16777219){ 
                    if(text_input.length == 0){
                        toPrevious(false)
                    }
                }
                // home ==> 16777232
                else if(event.key == 16777232){
                    toStart()
                }
                // end ==> 16777233
                else if(event.key == 16777233){
                    toEnd()
                }
            }

            Keys.onLeftPressed: {
                if(length == 0 || cursorPosition == 0){
                    toPrevious(true)
                }
                else{
                    cursorPosition --
                }
            }
            Keys.onRightPressed: {
                print(cursorPosition)
                if(length == 0 || cursorPosition == length){
                    toNext(true)
                }
                else{
                    cursorPosition ++
                }
            }
        }
    }

}

