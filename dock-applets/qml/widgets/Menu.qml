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

import QtQuick 2.0
import Deepin.DockApplet 1.0

DockMenu {
    content: getMenuContent()
    onActivate: {
        if(actionDict[id]){
            actionDict[id](id);
        }
    }

    function marshalJSON(value) {
        var valueJSON = JSON.stringify(value);
        return valueJSON
    }
    
    function unmarshalJSON(valueJSON) {
        var value = JSON.parse(valueJSON)
        return value
    }

    property string menuItems: getMenuItems()
    property var actionDict: new Object()

    function getMenuItems(){
        var a = new Array()
        return marshalJSON(a)
    }

    function getMenuContent(){
        var menuContent = {
            "checkableMenu": false,
            "singleCheck": false,
            "items": unmarshalJSON(menuItems)
        }
        return marshalJSON(menuContent)
    }

    function generateId(){
        return Object.keys(actionDict).length + ""
    }

    function addCheckboxItem(groupName, name, callback){
        var item_id = generateId();
        var realItemId = groupName + ":radio:" + item_id
        actionDict[realItemId] = callback;

        var menuItem = {
            "itemText": name,
            "itemId": realItemId,
            "isCheckable": true,
            "checked": false,
            "itemIcon": "",
            "itemIconHover": "",
            "isActive": true,
            "itemIconInactive": "",
            "showCheckmark": true,
            "itemSubMenu": {
                "checkableMenu": false,
                "singleCheck": false,
                "items": [
                ]
            }
        };
        var menuItems_obj = unmarshalJSON(menuItems)
        menuItems_obj.push(menuItem);
        menuItems = marshalJSON(menuItems_obj)
        return item_id
    }

    function addItem(name, callback){
        var item_id = generateId();
        actionDict[item_id] = callback;

        var menuItem = {
            "itemText": name,
            "itemId": item_id,
            "isCheckable": false,
            "checked": false,
            "itemIcon": "",
            "itemIconHover": "",
            "isActive": true,
            "itemIconInactive": "",
            "showCheckmark": true,
            "itemSubMenu": {
                "checkableMenu": false,
                "singleCheck": false,
                "items": [
                ]
            }
        };
        var menuItems_obj = unmarshalJSON(menuItems)
        menuItems_obj.push(menuItem)
        menuItems = marshalJSON(menuItems_obj)
        return item_id
    }
}
