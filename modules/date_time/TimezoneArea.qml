import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared/"

Column {
    id: timezoneArea
    width: parent.width
    height: {
        if(currentActionStateName == "addButton"){
            return addTimezoneListArea.height + 34
        }
        else{
            return userTimezoneListArea.height + 34
        }
    }

    Behavior on height {
        PropertyAnimation { duration: 150 }
    }

    property string currentActionStateName: ""
    property var listModelComponent: DListModelComponent {}

    property var timezoneCityDict: gDate.TimezoneCityList()
    property var userTimezoneList: gDate.userTimezoneList
    property string searchMd5: dbusSearch.NewTrieWithString(timezoneCityDict, "deepin-system-settings-timezone")

    property int listAreaMaxHeight

    function isInUserTimezoneList(s){
        for(var i=0; i<userTimezoneList.length; i++){
            if(s == userTimezoneList[i]){
                return true
            }
        }
        return false
    }

    onUserTimezoneListChanged: {
        if(userTimezoneList.length == 1 && titleLine.rightLoader.item){
            titleLine.rightLoader.item.currentActionStateName = ""
        }
    }

    function reset(){
        titleLine.rightLoader.item.currentActionStateName = ""
    }

    DBaseLine {
        id: titleLine
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Time zone")
        }

        rightLoader.sourceComponent: StateButtons {
            deleteButton.visible: userTimezoneList.length > 1
            onCurrentActionStateNameChanged: {
                // before changed
                if(timezoneArea.currentActionStateName == "addButton"){
                    for(var key in addTimezoneListView.selectedItemDict){
                        if(addTimezoneListView.selectedItemDict[key]){
                            gDate.AddUserTimezoneList(key)
                        }
                    }
                }
                // before changed

                timezoneArea.currentActionStateName = currentActionStateName

                // after changed
                if(timezoneArea.currentActionStateName == "addButton"){
                    searchInput.text = ""
                    addTimezoneListView.fillModel("")
                }
                // after changed
            }
        }

    }

    DSeparatorHorizontal {}

    Rectangle {
        id: userTimezoneListArea
        width: parent.width
        height: childrenRect.height
        color: dconstants.contentBgColor
        visible: timezoneArea.currentActionStateName != "addButton"

        ListView {
            id: userTimezoneListView
            width: parent.width
            height: model.count * 28 > listAreaMaxHeight ? listAreaMaxHeight : model.count * 28
            clip: true

            model: {
                var myListModel = listModelComponent.createObject(userTimezoneListView, {})
                for(var i in userTimezoneList){
                    var timezoneValue = userTimezoneList[i]
                    myListModel.append({
                        "item_id": timezoneValue,
                        "item_name": timezoneCityDict[timezoneValue]
                    })
                }
                return myListModel
            }

            delegate: SelectItem {
                totalItemNumber: userTimezoneListView.count
                selectItemId: windowView.stripString(gDate.currentTimezone)
                inDeleteAction: timezoneArea.currentActionStateName == "deleteButton"

                onDeleteAction: {
                    gDate.DeleteTimezoneList(itemId)
                }
                onSelectAction: {
                    gDate.SetTimeZone(itemId)
                }
            }

            DScrollBar {
                flickable: userTimezoneListView
            }
        }
    }

    Rectangle{
        id: addTimezoneListArea
        width: parent.width
        height: listAreaMaxHeight
        color: dconstants.contentBgColor
        visible: timezoneArea.currentActionStateName == "addButton"

        Rectangle {
            id: searchBox
            width: parent.width
            height: 28
            color: dconstants.contentBgColor

            DSearchInput {
                id: searchInput
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 30

                onTextChanged: {
                    addTimezoneListView.fillModel(text)
                }
            }
        }

        ListView {
            id: addTimezoneListView
            width: parent.width
            height: listAreaMaxHeight - 28
            anchors.top: searchBox.bottom
            anchors.left: parent.left
            clip: true

            property var selectedItemDict: new Object()

            function fillModel(s){
                selectedItemDict = new Object()
                var searchResult = dbusSearch.SearchKeys(s, searchMd5)
                var toSortResult = new Array()
                for(var i=0; i<searchResult.length; i++){
                    var timezoneValue = searchResult[i]
                    if(!isInUserTimezoneList(timezoneValue)){
                        var tmp = new Array()
                        tmp.push(timezoneValue)
                        tmp.push(timezoneCityDict[timezoneValue])
                        toSortResult.push(tmp)
                    }
                }
                toSortResult = windowView.sortSearchResult(toSortResult)
                model.clear()
                for(var i=0; i<toSortResult.length; i++){
                    var d = toSortResult[i]
                    model.append({
                        "value": d[0],
                        "label": d[1]
                    })
                }
            }

            model: ListModel {}
            delegate: AddTimezoneItem{
                onSelectAction: {
                    addTimezoneListView.selectedItemDict[itemValue] = selected
                    var length = 0
                    for(var key in addTimezoneListView.selectedItemDict){
                        if(addTimezoneListView.selectedItemDict[key]){
                            length += 1
                        }
                    }
                    if(length > 0){
                        titleLine.rightLoader.item.addLabel = dsTr("Add")
                    }
                    else{
                        titleLine.rightLoader.item.addLabel = dsTr("Close")
                    }
                }
            }

            DScrollBar {
                flickable: addTimezoneListView
            }
        }
    }
}
