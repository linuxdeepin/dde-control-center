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
        return Object.keys(actionDict).length
    }

    function addCheckboxItem(groupName, name, callback){
        var item_id = generateId();
        actionDict[item_id] = callback;

        var menuItem = {
            "itemText": name,
            "itemId": groupName + ":radio:" + item_id,
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
