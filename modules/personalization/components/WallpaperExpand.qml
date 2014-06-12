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
    id: wallpaper_themes_expand

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

            property string currentItemName: currentThemeObject.background

            function selectItem(itemValue){
                dbusThemeManager.Set("background", "", itemValue)
            }

            model: {
                //dbusThemeManager.backgroundList
                var myModel = listModelComponent.createObject(wallpapperView, {})
                var lists = dbusThemeManager.backgroundList
                for(var i in lists){
                    var name = lists[i]
                    myModel.append({
                        "item_img_url": dbusThemeManager.GetThumbnail("background", name),
                        "item_value": name
                    })
                }
                return myModel
            }

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
