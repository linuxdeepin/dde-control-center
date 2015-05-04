import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root

    rightLoader.sourceComponent: DComboBox {
        id: comboBox
        anchors.left: parent.left
        width: valueWidth
        menu.maxHeight: 200
        parentWindow: rootWindow

        onMenuSelect: {
            setKey(getAvailableValues()[index].Value)
        }
        itemDelegate: EditLineComboBoxMenuItem {
            property int index
            property bool itemOnHover
            property var value

            text: value
        }

        Connections {
            target: root
            onWidgetShown: {
                comboBox.value = root.getAvailableValuesTextByValue()
                comboBox.menu.model = root.getAvailableValuesText()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
            onCacheValueChanged: {
                comboBox.value = root.getAvailableValuesTextByValue()
                comboBox.menu.model = root.getAvailableValuesText()
                comboBox.selectIndex = root.getAvailableValuesIndex()
            }
        }
    }
}
