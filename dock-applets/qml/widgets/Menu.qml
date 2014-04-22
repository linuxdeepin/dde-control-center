import QtQuick 2.0
import Deepin.DockApplet 1.0

DockMenu {
    content: JSON.stringify(menuContent)
    onActivate: {
        if(actionDict[id]){
            actionDict[id]();
        }
    }

    property var menuContent: {
        "checkableMenu": false,
        "singleCheck": false,
        "items": menuItems
    }

    property var menuItems: []
    property var actionDict: new Object()

    function generateId(){
        return "action_" + Object.keys(actionDict).length
    }

    function addItem(name, callback){
        if(name != ""){
            var item_id = generateId();
            actionDict[item_id] = callback;
        }
        else{
            item_id = "-1";
        }
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
        menuItems.push(menuItem);
    }
}
