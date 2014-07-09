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

GridView {
    id: themeView
    width: parent.width
    height: childrenRect.height
    cellWidth: 144
    cellHeight: 112

    property string themeType: "gtk"

    property string currentItemName: "Deepin"
    property var itemList: new Array()

    property bool hasInit: false

    model: ListModel {}

    onItemListChanged: {
        if(hasInit){
            reloadModel()
        }
    }

    function reloadModel(){
        model.clear()
        for(var i in itemList){
            var item_name = itemList[i]
            var thumbnail = dbusThemeManager.GetThumbnail(themeType, item_name)

            if(!thumbnail){
                continue
            }

            var is_deletable = dbusThemeManager.GetFlag(themeType, item_name) == 1 ? true : false

            model.append({
                "item_img_url": thumbnail,
                "item_name": windowView.toHumanThemeName(item_name),
                "item_value": item_name,
                "is_deletable": is_deletable
            })
        }
        hasInit = true
    }

    Timer {
        id: delayInitTimer
        running: true
        interval: 500
        onTriggered: {
            reloadModel()
        }
    }

    delegate: ComponentThemeItem{
        width: themeView.cellWidth
        height: themeView.cellHeight
        selectedItemValue: currentItemName

        onSelectAction: {
            themeView.selectItem(itemValue)
        }
        onDeleteAction: {
            themeView.deleteItem(itemValue)
        }
    }

    function selectItem(itemValue){
        dbusThemeManager.Set(themeType, itemValue)
    }

    function deleteItem(itemValue){
        dbusThemeManager.Delete(themeType, itemValue)
    }

}
