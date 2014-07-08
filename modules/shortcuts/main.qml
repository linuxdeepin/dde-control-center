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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import DBus.Com.Deepin.Daemon.Search 1.0
import Deepin.Widgets 1.0
import "../shared/"

Item {
    id: shortcutsModule
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22
    property var listModelComponent: DListModelComponent {}

    property var dbusKeyBinding: KeyBinding {}
    property var searchId: Search {}

    property int currentShortcutId: -1
    property int expandItemIndex: -1

    property var stopSetKeyBinding: false

    property var categoryObjects: {
        "systemList": dsTr("System"),
        //"mediaList": dsTr("Sound and Media"), TODO: the backend is not available
        "windowList": dsTr("Window"),
        "workspaceList": dsTr("Workspace"),
        "customList": dsTr("Custom")
    }

    property string searchMd5
    property string currentSearchKeyword: ""
    property var systemKeybindingsDict: {
        var keywords = {}
        var allKeybindings = {}
        for(var key in categoryObjects){
            if(key=="customList") continue;
            for(var i in dbusKeyBinding[key]){
                var temp_list = dbusKeyBinding[key][i]
                keywords[temp_list[0]] = temp_list[1] + ", " + windowView.toHumanShortcutLabel(temp_list[2])
                temp_list.push(key)
                allKeybindings[temp_list[0]] = temp_list
            }
        }
        for(var i in dbusKeyBinding.customList){
            var temp_list = dbusKeyBinding.customList[i]
            keywords[temp_list[0]] = temp_list[1]
        }
        var retList = searchId.NewSearchWithDict(keywords)
        shortcutsModule.searchMd5 = retList[0]
        return allKeybindings
    }

    property var customKeyBindingsList: {
        var b = []
        for(var i in dbusKeyBinding.customList){
            var temp_list = dbusKeyBinding.customList[i]
            temp_list.push("customList")
            b.push(temp_list)
        }
        return b
    }

    function getKeyBindingInfo(id){
        var info = systemKeybindingsDict[id]
        if(info){
            return info
        }
        else{
            for(var i=0; i<customKeyBindingsList.length; i++){
                if(customKeyBindingsList[i][0]==id){
                    return customKeyBindingsList[i]
                }
            }
            return [-1, "", "", ""]
        }
    }

    Component.onCompleted: {
    }

    Column {
        id: keybindingTitleColumn
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle {
            text: modulesId.moduleLocaleNames["shortcuts"]
            rightLoader.sourceComponent: ResetButton {
                onClicked: {
                    dbusKeyBinding.Reset()
                }
            }
            rightLoader.visible: searchResultListView.keyword == ""
        }

        DSeparatorHorizontal {}

        DBaseLine{
            leftMargin: 18
            leftLoader.sourceComponent: DSearchInput {
                width: shortcutsModule.width - 36
                onTextChanged: {
                    searchResultListView.keyword = text
                }

                Keys.onEscapePressed: {
                    text = ""
                }
            }
        }

        DSeparatorHorizontal {}
    }

    DFlickable {
        anchors.top: keybindingTitleColumn.bottom
        width: parent.width
        height: parent.height - keybindingTitleColumn.height
        clip: true

        contentWidth: width
        contentHeight: keybindingListColumn.height

        Column {
            id: keybindingListColumn
            width: parent.width
            height: childrenRect.height
            clip: true

            Rectangle {
                id: searchResultListBox
                height: childrenRect.height
                width: parent.width
                color: dconstants.contentBgColor

                ListView {
                    id: searchResultListView
                    width: parent.width
                    height: searchResultListView.count * 30
                    focus: true
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    property string keyword: ""
                    property var keyData: {
                        var resultKeyBindings = new Array()
                        if(keyword){
                            var results = searchId.SearchString(keyword, searchMd5)
                            for(var i in results){
                                resultKeyBindings.push(getKeyBindingInfo(results[i]))
                            }
                        }
                        return resultKeyBindings
                    }

                    model: keyData.length
                    delegate: ShortcutInput {
                        info: searchResultListView.keyData[index]
                    }
                }
            }

            DSeparatorHorizontal {
                visible: searchResultListView.count != 0
            }

            Rectangle {
                id: systemCategoriesArea
                width: parent.width
                height: childrenRect.height
                color: dconstants.bgColor
                visible: searchResultListView.keyword == ""

                Column {
                    width: parent.width
                    height: childrenRect.height

                    Repeater {
                        id: systemShortcutCategoryList
                        width: parent.width
                        height: childrenRect.height
                        model: {
                            var myModel = listModelComponent.createObject(systemShortcutCategoryList, {})
                            for(var key in categoryObjects){
                                if(key=="customList") continue;
                                myModel.append({
                                    "name": categoryObjects[key],
                                    "propertyName": key
                                })
                            }
                            return myModel
                        }
                        delegate: ShortcutCategoryItem{}
                    }

                    CustomKeybinding {
                        id: customItem
                        property string name: dsTr("Custom")
                        property var keyBindings: dbusKeyBinding.customList
                        listMaxHeight: shortcutsModule.height - keybindingTitleColumn.height - keybindingListColumn.height
                    }
                }
            }
        }
    }
}
