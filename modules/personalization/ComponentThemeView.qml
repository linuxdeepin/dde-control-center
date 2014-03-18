import QtQuick 2.1

GridView {
    id: themeView
    width: parent.width
    height: childrenRect.height
    cellWidth: 144
    cellHeight: 112

    delegate: ComponentThemeItem{
        width: themeView.cellWidth
        height: themeView.cellHeight
        selectedItemValue: currentItemName

        onSelectAction: {
            themeView.selectItem(itemValue)
        }
    }
}
