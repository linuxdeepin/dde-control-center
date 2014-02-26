import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import DBus.Com.Deepin.Api.Search 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Flickable {
    id: shortcutsModule
    anchors.fill: parent
    contentWidth: width
    contentHeight: childrenRect.height

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22
    property var listModelComponent: DListModelComponent {}

    property var grabManagerId: GrabKey {}
    property var bindManagerId: KeyBinding {}
    property var searchId: Search {}

    property int currentShortcutId: -1
    property int expandItemIndex: -1
    property var conflictInvalid: bindManagerId.conflictInvalid
    property var conflictValid: bindManagerId.conflictValid

    property var categoryObjects: {
        "systemList": dsTr("System"),
        "mediaList": dsTr("Sound and Media"),
        "windowList": dsTr("Window"),
        "workSpaceList": dsTr("Workspace")
    }

    property string searchMd5
    property string currentSearchKeyword: ""
    property var searchObject: {
        var keywords = {}
        var allKeybindings = {}
        for(var key in categoryObjects){
            for(var i in bindManagerId[key]){
                var temp_list = bindManagerId[key][i]
                keywords[temp_list[0]] = temp_list[1]
                allKeybindings[temp_list[0]] = temp_list
            }
        }
        for(var i in bindManagerId.customList){
            var temp_list = bindManagerId.customList[i]
            keywords[temp_list[0]] = temp_list[1]
            allKeybindings[temp_list[0]] = temp_list
        }
        shortcutsModule.searchMd5 = searchId.NewTrieWithString(keywords, "deepin-system-settings.shortcuts")
        return allKeybindings
    }

    Column {
        id: keybindingTitleColumn
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle {
            text: dsTr("Shortcuts")
            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Reset")
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

    Column {
        id: keybindingListColumn
        anchors.top: keybindingTitleColumn.bottom
        width: parent.width
        height: childrenRect.height

        Rectangle {
            id: searchResultListBox
            height: childrenRect.height
            width: parent.width
            color: dconstants.contentBgColor

            ListView {
                id: searchResultListView
                width: parent.width
                height: {
                    if(shortcutsModule.height - keybindingTitleColumn.height - 2 < searchResultListView.count * 30){
                        return shortcutsModule.height - keybindingTitleColumn.height - 2
                    }
                    return searchResultListView.count * 30
                }
                focus: true
                clip: true

                Behavior on height {
                    NumberAnimation {duration: 100}
                }

                property string keyword: ""
                property var keyData: {
                    var resultKeyBindings = new Array()
                    if(keyword){
                        var results = searchId.SearchKeys(keyword, searchMd5)
                        for(var i in results){
                            resultKeyBindings.push(searchObject[results[i]])
                        }
                    }
                    return resultKeyBindings
                }

                model: keyData.length
                delegate: ShortcutInput {
                    info: searchResultListView.keyData[index]
                }
            }

            DScrollBar {
                flickable: searchResultListView
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
                            myModel.append({
                                "name": categoryObjects[key],
                                "propertyName": key
                            })
                        }
                        return myModel
                    }
                    delegate: ShortcutCategoryItem{}
                }

                CustomKeybindingExpand {
                    id: customItem
                    property string name: dsTr("Custom Shortcuts")
                    property var keyBindings: bindManagerId.customList
                    listMaxHeight: shortcutsModule.height - keybindingTitleColumn.height - keybindingListColumn.height
                }
            }
        }
    }

    Component.onCompleted: {
    }
}
