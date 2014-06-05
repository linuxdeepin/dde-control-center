import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditKey {
    id: root
    
    property var filter         // TODO
    
    DFileChooseInput {
        anchors.verticalCenter: parent.verticalCenter
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
            root.cacheValue = text
            setKey()
        }
        onFileChooseClicked: {
            fileChooseDialog.showWindow()
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

