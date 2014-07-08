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

import QtQuick 2.2
import Deepin.Widgets 1.0
import "../shared"

DBaseExpand {
    id: localeExpand

    property int listAreaMaxHeight: 100

    property string currentLocale: gDate.currentLocale
    property var localeToLanguage: {
        var allLocaleList = gDate.GetLocaleList()
        var r = new Object()
        for(var i in allLocaleList){
            var info = allLocaleList[i]
            r[info[0]] = info[1]
        }
        return r
    }
    property var allLocales: getAllLocales(false)
    property var allSearchLocales: getAllLocales(true)
    property string searchMd5: {
        var retList = dbusSearch.NewSearchWithDict(allSearchLocales)
        print(retList[0])
        return retList[0]
    }

    function getAllLocales(returnDict){
        if(returnDict){
            var toSortResult = new Object()
            for(var key in localeToLanguage) {
                toSortResult[key]= localeToLanguage[key]
            }
            return toSortResult
        }
        else{
            var toSortResult = new Array()
            for(var key in localeExpand.localeToLanguage) {
                var tmp = new Array()
                tmp.push(key)
                tmp.push(localeExpand.localeToLanguage[key])
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
                        var searchResult = dbusSearch.SearchString(text, searchMd5)
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

            Timer {
                id: initLocaleListTimer
                interval: 200
                onTriggered: {
                    localeListView.updateModel(allLocales)
                }
            }

            Component.onCompleted: {
                initLocaleListTimer.start()
            }

            DScrollBar {
                flickable: localeListView
            }
        }
    }

}

