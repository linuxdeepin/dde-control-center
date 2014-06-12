import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    width: parent.width
    property int contentHeight: 30
    property int centerPadding: 20
    property int titleWidth: 80
    property int sliderWidth: 198

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
            }
        }
    }

    DBaseLine {
        color: "transparent"
        rightLoader.sourceComponent: DTextButton {
            text: dsTr("Apply system wide")
        }
    }

}
