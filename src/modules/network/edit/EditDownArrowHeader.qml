import QtQuick 2.1
import Deepin.Widgets 1.0


DDownArrowHeader{
    onClicked: {
        if(activeExpandIndex == root.myIndex){
            activeExpandIndex = -1
        }
        else{
            activeExpandIndex = root.myIndex
        }
    }
    Component.onCompleted: {
        active = (activeExpandIndex == root.myIndex)
    }
}
