import QtQuick 2.2
import Deepin.Widgets 1.0
import "../shared"

DBaseExpand {
    id: localeExpand

    property int listAreaMaxHeight: 100

    property string currentLocale: gDate.currentLocale
    property var localeToLanguage: gDate.localeListMap
    property var allLocales: getAllLocales(false)
    property var allSearchLocales: getAllLocales(true)
    property string searchMd5: dbusSearch.NewTrieWithString(allSearchLocales, "deepin-system-settings-locales")

    function getAllLocales(returnDict){
        if(returnDict){
            var toSortResult = new Object()
            for(var key in localeToLanguage) {
                toSortResult[key]= localeToLanguage[key] + " (" + key + ")"
            }
            return toSortResult
        }
        else{
            var toSortResult = new Array()
            for(var key in localeExpand.localeToLanguage) {
                var tmp = new Array()
                tmp.push(key)
                tmp.push(localeExpand.localeToLanguage[key] + " (" + key + ")")
                toSortResult.push(tmp)
            }
            return windowView.sortSearchResult(toSortResult)
        }
    }

    header.sourceComponent: DDownArrowHeader {
        text: dsTr("Language")
        hintText: " (" + localeExpand.localeToLanguage[localeExpand.currentLocale]+ ")"
        width: 310
        anchors.leftMargin: 2
        anchors.rightMargin: 10

        Connections {
            target: localeExpand
            onExpandedChanged: {
                if(localeExpand.expanded != active){
                    active = localeExpand.expanded
                }
            }
        }
        
        onClicked: {
            localeExpand.expanded = !localeExpand.expanded
        }
    }

    content.sourceComponent: Rectangle {
        width: parent.width
        height: localeExpand.listAreaMaxHeight + searchInputBox.height
        color: dconstants.contentBgColor

        Connections {
            target: localeExpand
            onExpandedChanged: {
                if(expanded){
                    localeListView.positionViewAtIndex(localeListView.seletedIndex, ListView.Visible)
                }
            }
        }

        Item {
            id: searchInputBox
            width: parent.width
            height: 30

            DSearchInput {
                id: searchInput
                width: parent.width - 30
                anchors.centerIn: parent

                onTextChanged: {
                    //addTimezoneListView.fillModel(text)
                    if(text == ""){
                        localeListView.updateModel(allLocales)
                    }
                    else{
                        var searchResult = dbusSearch.SearchKeys(text, searchMd5)
                        var localeList = new Array()
                        for(var i in searchResult){
                            var tmp = new Array()
                            tmp.push(searchResult[i])
                            tmp.push(allSearchLocales[searchResult[i]])
                            localeList.push(tmp)
                        }
                        localeListView.updateModel(windowView.sortSearchResult(localeList))
                    }
                }
            }
        }

        ListView {
            id: localeListView
            anchors.top: searchInputBox.bottom
            width: parent.width
            height: parent.height - searchInputBox.height
            model: ListModel {}
            maximumFlickVelocity: 1000
            clip: true

            property int seletedIndex: -1

            delegate: SelectItem {
                totalItemNumber: 2
                selectItemId: localeExpand.currentLocale

                onSelectItemIdChanged: {
                    if(selectItemId == item_id){
                        ListView.view.seletedIndex = index
                    }
                }

                onSelectAction: {
                    gDate.SetLocale(itemId)
                }
            }

            function updateModel(localeList){
                model.clear()
                for(var i in localeList){
                    if(localeList[i][0] == localeExpand.currentLocale){
                        localeListView.seletedIndex = i
                    }
                    model.append({
                        "item_id": localeList[i][0],
                        "item_name": localeList[i][1]
                    })
                }
            }

            Component.onCompleted: {
                updateModel(allLocales)
            }

            DScrollBar {
                flickable: localeListView
            }
        }
    }
}

