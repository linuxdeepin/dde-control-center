import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    rightLoader.sourceComponent: DComboBox{
        id: comboBox
        anchors.left: parent.left
        anchors.leftMargin: -3
        width: valueWidth

        property var menuLabels: getAvailableValues()

        function menuSelect(i){
            root.value = menuLabels[i]
            setKey()
        }

        // TODO select current item when popup root menu firste time
        onClicked: {
            if(!rootMenu.visible){
                var pos = mapToItem(null, 0, 0)
                rootMenu.labels = comboBox.menuLabels
                rootMenu.requestMenuItem = comboBox
                rootMenu.posX = pos.x
                rootMenu.posY = pos.y + height
                rootMenu.innerWidth = width
            }
            rootMenu.visible = !rootMenu.visible
        }

        Component.onCompleted: {
            if (root.visible) {
                text = getKey()
                // TODO test
                print("EditLineComboBox.onCompleted:", root.section, root.key, root.value, "[", getAvailableValues(), "]")
            }
        }
    }
}
