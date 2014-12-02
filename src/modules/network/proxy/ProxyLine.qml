import QtQuick 2.0
import Deepin.Widgets 1.0

Row{
    spacing: 4
    property string proxyType: ""

    property alias host: hostText.text
    property alias port: portText.text

    signal proxyChanged
    onProxyChanged: {
        if(port == ""){
            dbusNetwork.SetProxy(proxyType, host, 0)
        }
        else{
            dbusNetwork.SetProxy(proxyType, host, port)
        }
    }
    Component.onCompleted: {
        var proxy = dbusNetwork.GetProxy(proxyType)
        host = proxy[0]
        if(parseInt(proxy[1]) == 0){
            port = ""
        }
        else{
            port = proxy[1]
        }
    }

    DTextInput {
        id: hostText
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - portLabel.width - portText.width - parent.spacing * 2 - 2
        onTextChanged: parent.proxyChanged()
    }
    DssH2 {
        id: portLabel
        text: dsTr("Port")
        anchors.verticalCenter: parent.verticalCenter
    }
    DTextInput {
        id: portText
        anchors.verticalCenter: parent.verticalCenter
        width: 40
        textInput.validator: IntValidator{}
        onTextChanged: {
            parent.proxyChanged()
        }
    }

}
