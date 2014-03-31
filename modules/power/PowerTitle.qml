import QtQuick 2.0
import Deepin.Widgets 1.0
import QtGraphicalEffects 1.0

DBaseLine {
    id: dssTitle
    width: parent.width
    height: 48

    property string text: ""
    property string hint: ""
    property bool breath: false
    property bool showHyphen: false

    leftLoader.sourceComponent: Row {
        DssH1 {
            id: title
            font.bold: true
            color: "white"
            text: dssTitle.text
        }
        
        DssH1 {
            visible: dssTitle.showHyphen
            font.bold: true
            color: "#666666"
            text: " - "
        }        
        
        Item {
            id: hint_box
            width: hint.width
            height: title.height
            
            DssH2 {
                id: hint
                font.bold: true
                color: "white"
                text: dssTitle.hint
                anchors.verticalCenter: parent.verticalCenter

                SequentialAnimation {
                    running: dssTitle.breath
                    loops: Animation.Infinite
                    NumberAnimation { target: hint_box; property: "opacity"; to: 0.2; duration: 1200 }
                    NumberAnimation { target: hint_box; property: "opacity"; to: 1.0; duration: 1200 }
                }
            }
            
            Glow {
                visible: breath
                anchors.fill: hint
                radius: 4
                samples: 8
                color: "white"
                source: hint
            }
        }
    }
}
