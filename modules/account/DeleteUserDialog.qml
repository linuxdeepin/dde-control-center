import QtQuick 2.1
import "../widgets"

Column {
    
    property variant constants: DConstants{}
    
    Rectangle {
        width: parent.width
        height: parent.height / 2
        color: constants.bgColor
    }
    
    DSeparatorHorizontal{}

    Rectangle {
        width: parent.width
        height: parent.height / 2
        color: constants.bgColor
    }
}