import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine{
    id: root
    
    property var filter         // TODO
    
    rightLoader.sourceComponent: DFileChooseInput {
        width: valueWidth
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
        currentFolder: windowView.getHomeDir()
        onSelectAction: {
            root.rightLoader.item.text = fileUrl
        }
    }
}

