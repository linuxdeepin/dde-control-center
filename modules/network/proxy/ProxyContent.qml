import QtQuick 2.0
import Deepin.Widgets 1.0
import "../../shared"

Column {
    id: proxyContent
    width: parent.width
    property int contentHeight: 30
    property int centerPadding: 20
    property int titleWidth: 80
    property int sliderWidth: 198
    property var listModelComponent: DListModelComponent {}

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

    Item {
        id: methodLine
        height: 30
        width: parent.width

        GridView{
            id: methodView
            width: parent.width
            height: 30

            cellWidth: width/3
            cellHeight: 30

            boundsBehavior: Flickable.StopAtBounds

            model: {
                var model = listModelComponent.createObject(methodView, {})
                model.append({
                    "item_label": dsTr("None"),
                    "item_value": 0,
                    "item_tooltip": ""
                });
                model.append({
                    "item_label": dsTr("Manual"),
                    "item_value": 1,
                    "item_tooltip": ""
                });
                model.append({
                    "item_label": dsTr("Auto"),
                    "item_value": 2,
                    "item_tooltip": ""
                });
                return model
            }

            delegate: PropertyItem {
                currentValue: currentIndex
                onSelectAction: {
                    currentIndex = itemValue
                    if(currentIndex == 0){
                        dbusNetwork.SetProxyMethod(proxyMethods[0])
                    }
                }
            }
        }
    }

    Column {
        id: manualProxyColumn
        width: parent.width
        visible: currentIndex == 1

        DSeparatorHorizontal {}
        Item {
            width: parent.width
            height: 6
        }

        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: proxyContent.centerPadding
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
            centerPadding: proxyContent.centerPadding
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
            centerPadding: proxyContent.centerPadding
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
            centerPadding: proxyContent.centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Socks Proxy")
            content.sourceComponent: ProxyLine {
                width: sliderWidth
                height: contentHeight
                proxyType: "socks"
            }
        }
    }

    Column {
        width: parent.width
        visible: currentIndex == 2

        DSeparatorHorizontal {}

        Item {
            width: parent.width
            height: 6
        }

        DCenterLine {
            color: "transparent"
            height: contentHeight
            centerPadding: proxyContent.centerPadding
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
        visible: currentIndex != 0
        color: "transparent"
        rightLoader.sourceComponent: DTextButton {
            text: dsTr("Apply system wide")
            onClicked: {
                dbusNetwork.SetProxyMethod(proxyMethods[currentIndex])
            }
        }
    }

}
