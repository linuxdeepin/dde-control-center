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
    }
    
    DFileChooseDialog {
        id: fileChooseDialog
        currentFolder: mainObject.getHomeDir()
        showHidden: true
        onSelectAction: {
            root.rightLoader.item.text = fileUrl
        }
        onIsVisibleChanged: {
            rootWindow.setPanelHidable(!isVisible)
        }
    }
}

