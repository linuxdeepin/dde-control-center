import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    id: trayIconButton
    
    property int tabIndex: 0
    property string rightboxId: moduleId
    property string iconPathHeader: "trayicons/" + moduleId
    property string extStr: moduleId == "system_info" ? ".png": ".svg"
    
    property bool hover: false
    property url iconPath: '../../modules/' + moduleId + '/iconPlugin.qml'

    signal clicked

    QtObject {
        id: defaultIcon
        property url normalImage: iconPathHeader + '_normal' + extStr
        property url hoverImage: iconPathHeader + '_hover' + extStr
        property url pressImage: hoverImage
    }

    Loader {
        id: pluginLoader 
        source: iconPath
        property bool showWarning: false
        property int tipNumber: 0
        property var icon: defaultIcon
    }

    Image {
        id: iconImage
        anchors.centerIn: parent
        width: 28
        height: 28
        source: pluginLoader.icon.normalImage
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
            trayIconButton.ListView.view.currentIndex = index
            parent.clicked()
        }
        onEntered: {
            iconImage.source = pluginLoader.icon.hoverImage
            parent.hover = true
        }
        onExited: {
            iconImage.source = pluginLoader.icon.normalImage
            parent.hover = false
        }
        onPressed: {
            iconImage.source = pluginLoader.icon.pressImage
            parent.hover = false
        }
        onReleased: {
            iconImage.source= containsMouse ? pluginLoader.icon.hoverImage : pluginLoader.icon.normalImage
            parent.hover = containsMouse
        }
    }

    onHoverChanged: {
        trayIconTip.isHover = hover
        if (hover){
            root.trayIconHoverHandler(moduleId, index)
        }
        else {
            trayIconTip.timeOutHide.restart()
        }
    }
}
