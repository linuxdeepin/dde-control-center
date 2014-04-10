import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: cursor_themes_expand

    myIndex: 4
    title: dsTr("Cursor")

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: childrenRect.height

        ComponentThemeView {
            id: cursorList
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

            property string currentItemName: currentThemeObject.cursorTheme

            function selectItem(itemValue){
                dbusThemeManager.SetCursorTheme(itemValue)
            }

            model: {
                var myModel = listModelComponent.createObject(cursorList, {})
                var themes = dbusThemeManager.cursorThemeList
                for(var i in themes){
                    var theme_id = themes[i]
                    myModel.append({
                        "item_img_url": dbusThumbPath.CursorPath(theme_id),
                        "item_name": windowView.toHumanThemeName(theme_id),
                        "item_value": theme_id
                    })
                }
                return myModel
            }
        }
        
    }
}
