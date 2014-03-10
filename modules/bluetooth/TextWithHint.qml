import QtQuick 2.1

Row {
    id: _root_
    property string text: "Title"
    property string hint: "Hint"
    property color textColor: "white"
    property color hintColor: "black"
    property int textSize: 13
    property int hintSize: 13
    
    Text {
        text: _root_.text
        color: _root_.textColor
        font.pixelSize: _root_.textSize
    }
    
    Text {
        text: "(" + _root_.hint + ")"
        color: _root_.hintColor
        font.pixelSize: _root_.hintSize
    }
}