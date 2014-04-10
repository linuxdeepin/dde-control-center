import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: icon_themes_expand

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: childrenRect.height
        
        ComponentThemeView {
            id: iconList
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

            property string currentItemName: currentThemeObject.iconTheme

            function selectItem(itemValue){
                dbusThemeManager.SetIconTheme(itemValue)
            }

            model: {
                var myModel = listModelComponent.createObject(iconList, {})
                var themes = dbusThemeManager.iconThemeList
                for(var i in themes){
                    var theme_id = themes[i]
                    myModel.append({
                        "item_img_url": dbusThumbPath.IconPath(theme_id),
                        "item_name": windowView.toHumanThemeName(theme_id),
                        "item_value": theme_id
                    })
                }
                return myModel
            }
        }

    }
}

