import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine{
    id: root
    
    property var filter         // TODO
    
    rightLoader.sourceComponent: DFileChooseInput {
        id: fileChooseInput
        activeFocusOnTab: true
        width: valueWidth
        state: root.showError ? "warning" : "normal"
        Connections {
            target: root
            onWidgetShown: {
                text = root.cacheValue
            }
            onCacheValueChanged: {
                text = root.cacheValue
            }
        }
        onTextChanged: {
            setKey(text)
        }
        onFileChooseClicked: {
            fileChooseDialog.showWindow()
        }
        Rectangle {
            radius: 3
            color: "transparent"
            border.color: root.showError ? errorColor : normalBorderColor
            anchors.fill: fileChooseInput
        }
    }
    
    DFileChooseDialog {
        id: fileChooseDialog
        currentFolder: windowView.getHomeDir()
        onSelectAction: {
            root.rightLoader.item.text = fileUrl
        }
    }
}

