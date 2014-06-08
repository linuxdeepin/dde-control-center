import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO clickable pacakge text, and jump to software center
    
    // leftLoader.sourceComponent: Row {
    //     spacing: 8
    //     DssH2 {
    //         // color: errorColor
    //         text: dsTr("Missing Pacakge")
    //     }
    
    //     DssH2 {
    //         id: parent
    //         color: errorColor
    //         font.underline: true
    //         Connections {
    //             target: root
    //             onWidgetShown: {
    //                 text = root.cacheValue
    //             }
    //             onCacheValueChanged: {
    //                 text = root.cacheValue
    //             }
    //         }
    //         MouseArea {
    //             z: -1
    //             anchors.fill: parent
    //             cursorShape: Qt.PointingHandCursor
    //             onClicked: {
    //                 print("-> link", root.cacheValue)
    //             }
    //         }
    //     }
    // }
    
    leftLoader.sourceComponent: DssH2 {
        // color: errorColor
        text: dsTr("Missing Pacakge")
    }
    
    rightLoader.sourceComponent: DssH2 {
        id: parent
        color: errorColor
        font.underline: true
        Connections {
            target: root
            onWidgetShown: {
                text = root.cacheValue
            }
            onCacheValueChanged: {
                text = root.cacheValue
            }
        }
        MouseArea {
            z: -1
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                print("-> link", root.cacheValue)
            }
        }
    }
}    

