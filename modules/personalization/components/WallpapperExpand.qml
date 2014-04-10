import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: wallpaper_themes_expand

    myIndex: 3
    title: dsTr("Wallpaper")

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: childrenRect.height

        GridView {
            id: wallpapperView
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

            cellWidth: personalizationModule.cellWidth
            cellHeight: personalizationModule.cellHeight - 14

            property string currentItemName: currentThemeObject.backgroundFile

            function selectItem(itemValue){
                dbusThemeManager.SetBackgroundFile(itemValue)
            }

            model: dbusThemeManager.backgroundList

            delegate: WallpapperItem {
                width: cellWidth
                height: cellHeight - 14
                selectedItemValue: wallpapperView.currentItemName

                onSelectAction: {
                    wallpapperView.selectItem(itemValue)
                }

            }
        }
    }
}
