import QtQuick 2.1
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0

Item {
    id: trayIconButton
    
    property string rightboxId: moduleId
    property string iconPathHeader: "trayicons/" + moduleId
    property string extStr: moduleId == "system_info" ? ".png": ".svg"
    
    property bool hover: false
    property url iconPath: windowView.isIconPluginExist(moduleId) ? '../../modules/' + moduleId + '/iconPlugin.qml' : ''

    QtObject {
        id: defaultIcon
        property url normalImage: iconPathHeader + '_normal' + extStr
        property url hoverImage: iconPathHeader + '_press' + extStr
        property url pressImage: iconPathHeader + '_press' + extStr
    }

    Loader {
        id: pluginLoader 
        source: iconPath
        property bool showWarning: false
        property int tipNumber: 0
        property var icon: defaultIcon
    }

    Column {
        anchors.fill: parent
        Item {
            width: parent.width
            height: parent.height

            Image {
                id: iconImage
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: 28
                height: 28
                source: {
                    if(trayIconButton.ListView.view.currentIndex == index){
                        return pluginLoader.icon.pressImage
                    }
                    else if(trayIconButton.hover){
                        return pluginLoader.icon.hoverImage
                    }
                    return pluginLoader.icon.normalImage
                }
            }

            DLabel {
                id: moduleName
                anchors.left: iconImage.right
                anchors.leftMargin: 27
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 14
                text: modulesId.moduleLocaleNames[moduleId]
                color: trayIconButton.hover ? Qt.rgba(0, 144/255, 1, 1.0) : dconstants.fgColor

            }

            Image {
                id: warning
                anchors.top: parent.top
                anchors.topMargin: 2
                anchors.left: parent.left
                anchors.leftMargin: 2
                source: "images/warning.svg"
                visible: pluginLoader.showWarning
            }

            NumberTip {
                id: numberTip
                anchors.top: parent.top
                anchors.topMargin: 2
                anchors.left: parent.left
                anchors.leftMargin: 2
                visible: pluginLoader.tipNumber != 0
                currentNumber: pluginLoader.tipNumber
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                }
                onEntered: {
                    //iconImage.source = pluginLoader.icon.hoverImage
                    trayIconButton.hover = true
                }
                onExited: {
                    trayIconButton.hover = false
                }
                onPressed: {
                    //iconImage.source = pluginLoader.icon.pressImage
                    trayIconButton.ListView.view.currentIndex = index
                    trayIconButton.hover = false
                    trayIconButton.ListView.view.iconClickAction(trayIconButton.rightboxId)
                }
                onReleased: {
                    trayIconButton.hover = containsMouse
                }
            }

        }

        //DSeparatorHorizontal {}
    }


    onHoverChanged: {
        trayIconTip.isHover = hover
        if (hover){
            panelContent.trayIconHoverHandler(moduleId, index)
        }
        else {
            trayIconTip.timeOutHide.restart()
        }
    }
}
