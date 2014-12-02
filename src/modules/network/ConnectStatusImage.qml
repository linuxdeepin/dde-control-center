import QtQuick 2.0

Image{
    id: connectStatusImage

    property int currentStatusCode: 20

    onCurrentStatusCodeChanged:{
    }

    property string connectType: "wired"

    property var codeToImageDict: {
        20: "images/wired_status_error.svg",
        30: "images/wired_status_offline.svg",
        70: "images/wired_status_transmit-receive.svg",
        90: "images/wired_status_idle.svg",
        100: "images/wired_status_transmit.svg"
    }
    
    source: codeToImageDict[currentStatusCode]
}
