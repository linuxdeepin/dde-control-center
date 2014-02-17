import QtQuick 2.1
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0

Rectangle {
    id: moduleIconItem
    clip: true
    
    width: GridView.view.itemSize
    height: GridView.view.itemSize
    color: Qt.rgba(1, 1, 1, 0)

    property bool isSelected: rightBoxLoaderItem.iconId == itemId

    onIsSelectedChanged: {
        if(isSelected){
            state = "selected"
        }
        else{
            state = ""
        }
    }

    property string itemId: moduleId
    property string iconPathHeader: "trayicons/" + moduleId
    property string extStr: {
        if(moduleId == "system_info" | moduleId == "grub" | moduleId == "home"){
            return ".png"
        }
        else{
            return ".svg"
        }
    }
    
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

    states: [
        State {
            name: "hovered"
            PropertyChanges { target: outBox; color: Qt.rgba(1, 1, 1, 0.1); }
            PropertyChanges { target: moduleIcon; source: pluginLoader.icon.hoverImage }
            PropertyChanges { target: moduleName; color: Qt.rgba(0, 144/255, 1, 1.0) }
        },
        State {
            name: "selected"
            PropertyChanges { target: outBox; color: Qt.rgba(1, 1, 1, 0);}
            PropertyChanges { target: moduleIcon; source: pluginLoader.icon.hoverImage; anchors.topMargin: 8 }
            PropertyChanges { target: moduleName; visible: false }
        }
    ]

    function changeState(s){
        if(moduleIconItem.state != "selected"){
            moduleIconItem.state = s
        }
    }

    Rectangle {
        id: outBox
        property int size: parent.width - 6
        width: size
        height: size
        anchors.centerIn: parent
        radius: 3
        color: Qt.rgba(1, 1, 1, 0)

        Image {
            id: moduleIcon
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: moduleName.visible ? -16 : 0
            source: pluginLoader.icon.normalImage
        }

        DLabel {
            id: moduleName
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: moduleIcon.bottom
            anchors.topMargin: 6
            width: parent.width - 4
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
            text: modulesId.moduleLocaleNames[moduleId]
            elide: Text.ElideRight
            visible: opacity != 0
            opacity: moduleIconItem.GridView.view.iconLabelOpacity
            //visible: false
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
                moduleIconItem.GridView.view.iconClickAction(index, moduleIconItem.itemId)
            }
            onEntered: {
                changeState("hovered")
                moduleIconItem.hover = true
            }
            onExited: {
                changeState("")
                moduleIconItem.hover = false
            }
            onPressed: {
            }
            onReleased: {
                if(containsMouse){
                    changeState("hovered")
                    moduleIconItem.hover = true
                }
                else{
                    changeState("")
                    moduleIconItem.hover = false
                }
            }
        }

    }

    onHoverChanged: {
        trayIconTip.isHover = hover
        if(trayIconTip.isHover){
            panelContent.trayIconHoverHandler(moduleId, index)
        }
        else {
            trayIconTip.timeOutHide.restart()
        }
    }
}
