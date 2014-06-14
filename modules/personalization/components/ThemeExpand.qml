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
    id: themes_expand

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: themeView.height

        GridView {
            id: themeView
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

            cellWidth: personalizationModule.cellWidth
            cellHeight: personalizationModule.cellHeight

            property string currentItemName: dbusThemeManager.currentTheme

            function selectItem(itemValue){
                dbusThemeManager.Set("theme", itemValue)
            }

            Connections{
                target: dbusThemeManager
                onThemeListChanged: {
                    themeView.load_theme_list()
                }
            }

            function load_theme_list(){
                themeView.model.clear()
                for(var i in themeObjectList){
                    var themeObj = themeObjectList[i]

                    themeView.model.append({
                        "item_img_url": dbusThemeManager.GetThumbnail("theme", themeObj.name),
                        "item_name": themeObj.name,
                        "item_value": themeObj.name,
                        "themeObj": themeObj
                    })
                }
            }

            Component.onCompleted: load_theme_list()

            model: ListModel{}

            delegate: ThemeItem {
                width: cellWidth
                height: cellHeight
                selectedItemValue: themeView.currentItemName

                onSelectAction: {
                    themeView.selectItem(itemValue)
                }

                onPreviewAction:{
                    if (previewsWindow.isVisible && previewsWindow.themeObject == themeObj){
                        previewsWindow.hideWindow()
                    }
                    else{
                        previewsWindow.showWindow(themeObj)
                    }
                }
            }
        } // end of GridView
    }
}
