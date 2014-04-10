import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: window_themes_expand

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: childrenRect.height

        ComponentThemeView {
            id: gtkList
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

            property string currentItemName: currentThemeObject.gtkTheme

            function selectItem(itemValue){
                dbusThemeManager.SetGtkTheme(itemValue)
            }

            model: {
                var myModel = listModelComponent.createObject(gtkList, {})
                var themes = dbusThemeManager.gtkThemeList
                for(var i in themes){
                    var theme_id = themes[i]
                    myModel.append({
                        "item_img_url": dbusThumbPath.GtkPath(theme_id),
                        "item_name": windowView.toHumanThemeName(theme_id),
                        "item_value": theme_id
                    })
                }
                return myModel
            }
        }
        
    }
}
