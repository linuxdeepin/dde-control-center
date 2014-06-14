/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
                dbusThemeManager.Set("gtk", itemValue)
            }

            model: {
                var myModel = listModelComponent.createObject(gtkList, {})
                var themes = dbusThemeManager.gtkThemeList
                for(var i in themes){
                    var theme_id = themes[i]
                    var thumbnail = dbusThemeManager.GetThumbnail("gtk", theme_id)

                    if(!thumbnail){
                        continue
                    }

                    myModel.append({
                        "item_img_url": thumbnail,
                        "item_name": windowView.toHumanThemeName(theme_id),
                        "item_value": theme_id
                    })
                }
                return myModel
            }
        }
        
    }
}
