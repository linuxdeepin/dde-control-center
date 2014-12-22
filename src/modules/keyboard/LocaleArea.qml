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
import DBus.Com.Deepin.Daemon.LangSelector 1.0
import "../shared"

DBaseExpand {
    id: localeExpand

    property int listAreaMaxHeight: 100

    property var dbusLangSelector: LangSelector{}
    property var listModelComponent: DListModelComponent {}
    property string currentLocale: dbusLangSelector.currentLocale

    property var allLocaleInformation: dbusLangSelector.GetLocaleList()
    property var localeToLanguage: {
        var tmp = new Object()
        for(var i in allLocaleInformation){
            var info = allLocaleInformation[i]
            tmp[info[0]] = info[1]
        }
        return tmp
    }

    property string searchMd5: ""
    property var languageList: null
    property var languageToLocale: null

    header.sourceComponent: DownExpandHeader{
        text: dsTr("Language")
        hintTextId: currentLocale
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
                    if(text == ""){
                        localeListView.localeList = allLocaleInformation
                    }
                    else{
                        var searchResult = dbusSearch.SearchString(text, searchMd5)
                        var localeList = new Array()
                        for(var i in searchResult){
                            var label = searchResult[i]
                            var code = languageToLocale[label]
                            localeList.push([code, label])
                        }
                        localeListView.localeList = localeList
                    }
                }
            }
        }

        ListView {
            id: localeListView
            anchors.top: searchInputBox.bottom
            width: parent.width
            height: parent.height - searchInputBox.height
            model: getModel(localeList)
            clip: true
            //init scroll to selected item
            highlightFollowsCurrentItem: true
            currentIndex: count -1

            property int seletedIndex: -1

            property var localeList: new Array()

            delegate: LangSelectItem {
                totalItemNumber: 2
                selectItemId: localeExpand.currentLocale

                onSelectItemIdChanged: {
                    if(selectItemId == item_id){
                        ListView.view.seletedIndex = index
                    }
                }

                onSelectAction: {
                    dbusLangSelector.SetLocale(itemId)
                }
            }

            function getModel(localeList){
                var newModel = listModelComponent.createObject(localeListView, {})
                for(var i in localeList){
                    newModel.append({
                        "item_id": localeList[i][0],
                        "item_name": localeList[i][1]
                    })
                }
                return newModel
            }

            DScrollBar {
                flickable: localeListView
            }

            function initSearchLocaleData(){
                languageList = new Array()
                languageToLocale = new Object()
                for(var i in allLocaleInformation){
                    var info = allLocaleInformation[i]
                    languageList.push(info[1])
                    languageToLocale[info[1]] = info[0]
                    if(info[0] == localeExpand.currentLocale){
                        localeListView.seletedIndex = i
                    }
                }
                searchMd5 = dbusSearch.NewSearchWithStrList(languageList)[0]
                localeListView.localeList = allLocaleInformation
            }

            Timer {
                id: initSearchLocaleDataTimer
                running: true
                interval: 1000
                onTriggered: {
                    localeListView.initSearchLocaleData()
                }
            }
        }
    }
}

