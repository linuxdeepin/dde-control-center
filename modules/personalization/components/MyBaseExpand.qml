import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseExpand {
    id: myBaseExpand

    property int myIndex: 0
    property string title: header.item.text

    expanded: activeExpandIndex == myIndex
    onExpandedChanged: {
        if(header.item){
            header.item.active = expanded
        }
    }

    header.sourceComponent: DDownArrowHeader {
        onClicked: {
            if(activeExpandIndex == root.myIndex){
                activeExpandIndex = -1
            }
            else{
                activeExpandIndex = root.myIndex
            }
        }
    }

}
