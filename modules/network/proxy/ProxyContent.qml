import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    property int contentHeight: 30
    property int centerPadding: 20
    property int titleWidth: 80
    property int sliderWidth: 198

    property var proxyMethods: ["none", "manual", "auto"]
    property var currentIndex: {
        var method = dbusNetwork.GetProxyMethod()
        for(var i in proxyMethods){
            if(proxyMethods[i] == method){
                return i
            }
        }
        return 0
    }

    DCenterLine {
        id: methodLine
        color: "transparent"
        height: contentHeight
        centerPadding: centerPadding
        leftWidth: titleWidth
        title.text: dsTr("Method")
        property int methodId: {
            if(content.item){
                return content.item.selectIndex
            }
            else{
                return 0
            }
        }
        content.sourceComponent: DComboBox{
            width: sliderWidth
            parentWindow: rootWindow
            selectIndex: currentIndex
            menu.labels: {
                var arr = new Array()
                arr.push("None")
                arr.push("Manual")
                arr.push("Automatic")
                return arr
            }
        }
    }

    Column {
        id: manualProxyColumn
        width: parent.width
        visible: methodLine.methodId == 1

        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("HTTP Proxy")
            content.sourceComponent: ProxyLine{
                width: sliderWidth
                height: contentHeight
                proxyType: "http"
            }
        }
        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("HTTPS Proxy")
            content.sourceComponent: ProxyLine {
                width: sliderWidth
                height: contentHeight
                proxyType: "https"
            }
        }
        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("FTP Proxy")
            content.sourceComponent: ProxyLine {
                width: sliderWidth
                height: contentHeight
                proxyType: "ftp"
            }
        }
        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Socks Host")
            content.sourceComponent: ProxyLine {
                width: sliderWidth
                height: contentHeight
                proxyType: "socks"
            }
        }
    }

    Column {
        width: parent.width
        visible: methodLine.methodId == 2
        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Configuration URL")
            content.sourceComponent: DTextInput {
                width: sliderWidth
                onTextChanged: {
                    dbusNetwork.SetAutoProxy(text)
                }
                Component.onCompleted: {
                    text = dbusNetwork.GetAutoProxy()
                }
            }
        }
    }

    DBaseLine {
        color: "transparent"
        rightLoader.sourceComponent: DTextButton {
            text: dsTr("Apply system wide")
            onClicked: {
                dbusNetwork.SetProxyMethod(proxyMethods[methodLine.methodId])
            }
        }
    }

}
