import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQml.Models 2.1
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import DBus.Com.Deepin.Api.Search 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Item {
    id: shortcutsModule
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22

    property var grabManagerId: GrabKey {}
    property var bindManagerId: KeyBinding {}
    property var searchId: Search {}

    property int currentShortcutId: -1
    property int expandItemIndex: -1
    property var conflictInvalid: bindManagerId.conflictInvalid
    property var conflictValid: bindManagerId.conflictValid

    property string searchMd5
    property string currentSearchKeyword: ""
    property var searchObject: {
        var keywords = {}
        var allKeybindings = {}
        var d = ['customList', "systemList", 'mediaList', 'windowList', 'workSpaceList']
        for(var i in d){
            for(var j in bindManagerId[d[i]]){
                var temp_list = JSON.parse(JSON.stringify(bindManagerId[d[i]][j]))
                keywords[temp_list[0]] = temp_list[1]
                allKeybindings[temp_list[0]] = temp_list
            }
        }
        shortcutsModule.searchMd5 = searchId.NewTrieWithString(keywords, "deepin-system-settings.shortcuts")
        return {
            "keywords": JSON.parse(JSON.stringify(keywords)),
            "keyBindings": JSON.parse(JSON.stringify(allKeybindings))
        }
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
                    NumberAnimation {duration: 200}
                }

                property string keyword: ""
                property var keyData: {
                    var resultKeyBindings = new Array()
                    if(keyword){
                        var results = searchId.SearchKeys(keyword, searchMd5)
                        for(var i in results){
                            resultKeyBindings.push(searchObject["keyBindings"][results[i]])
                        }
                    }
                    return resultKeyBindings
                }

                model: keyData.length
                delegate: ShortcutInput {
                    info: searchResultListView.keyData[index]
                    warning: {
                        for(var i in conflictValid){
                            if (info[0] == conflictValid[i]){
                                return "conflict"
                            }
                        }
                        for(var i in conflictInvalid){
                            if (info[0] == conflictInvalid[i]){
                                return "error"
                            }
                        }
                        return ""
                    }
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
            id: allCategoriesArea
            width: parent.width
            height: content.height
            color: dconstants.bgColor
            visible: searchResultListView.keyword == ""

            property var expandItems: ObjectModel {
                ObjectModelItem {
                    id: systemItem
                    property string name: dsTr("System")
                    property var myIndex: ObjectModel.index
                    property var keyBindings: bindManagerId.systemList
                }

                ObjectModelItem {
                    id: mediaItem
                    property string name: dsTr("Sound and Media")
                    property var myIndex: ObjectModel.index
                    property var keyBindings: bindManagerId.mediaList
                }

                ObjectModelItem {
                    id: windowItem
                    property string name: dsTr("Window")
                    property var myIndex: ObjectModel.index
                    property var keyBindings: bindManagerId.windowList
                }

                ObjectModelItem {
                    id: workspaceItem
                    property string name: dsTr("Workspace")
                    property var myIndex: ObjectModel.index
                    property var keyBindings: bindManagerId.workSpaceList
                }
            }

            Column {
                id: content
                anchors.top: parent.top
                width: parent.width
                
                Repeater {
                    id: repeater
                    model: allCategoriesArea.expandItems
                }

                CustomKeybindingExpand {
                    id: customItem
                    property string name: dsTr("Custom Shortcuts")
                    property var keyBindings: bindManagerId.customList
                }
            } // End of content
        }

    }
}
