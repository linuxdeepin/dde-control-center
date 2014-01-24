import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQml.Models 2.1
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import DBus.Com.Deepin.Dde.Api.Pinyin 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Item {
    id: default_applications
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22

    property var grabManagerId: GrabKey {}
    property var bindManagerId: KeyBinding {}
    property var pinyinId: PinyinTrie {}

    property int currentShortcutId: -1
    property int expandItemIndex: -1
    property var conflictInvalid: bindManagerId.conflictInvalid
    property var conflictValid: bindManagerId.conflictValid
    property string searchMd5: pinyinId.NewTrieWithString(searchObject.keywords, "deepin-system-settings.shortcuts")

    property var searchObject: {
        var keywords = new Array()
        var allKeybindings = new Array()
        for(var i in bindManagerId.systemList){
            keywords.push(bindManagerId.systemList[i][1])
            allKeybindings.push(bindManagerId.systemList[i])
        }
        for(var i in bindManagerId.mediaList){
            keywords.push(bindManagerId.mediaList[i][1])
            allKeybindings.push(bindManagerId.mediaList[i])
        }
        for(var i in bindManagerId.windowList){
            keywords.push(bindManagerId.windowList[i][1])
            allKeybindings.push(bindManagerId.windowList[i])
        }
        for(var i in bindManagerId.workSpaceList){
            keywords.push(bindManagerId.workSpaceList[i][1])
            allKeybindings.push(bindManagerId.workSpaceList[i])
        }
        for(var i in bindManagerId.customList){
            keywords.push(bindManagerId.customList[i][1])
            allKeybindings.push(bindManagerId.customList[i])
        }
        var all = new Object()
        all['keywords'] = keywords
        all['keyBindings'] = allKeybindings
        return all
    }

    Column {
        id: keybindingTitleColumn
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle {
            text: dsTr("Shortcuts")
            rightLoader.sourceComponent: Item {
                height: 38
                width: 160

                property bool handHide: false

                NumberAnimation {
                    id: showSearchEntyArea
                    target: searchEntryArea
                    properties: "width"
                    to: 160
                    duration: 400

                    onStarted: {
                        searchInput.forceActiveFocus()
                        searchButtonArea.visible = false
                    }
                }

                NumberAnimation {
                    id: hideSearchEntyArea
                    target: searchEntryArea
                    property: "width"
                    to: 0
                    duration: 400

                    onStarted: {
                        handHide = true
                    }

                    onStopped: {
                        searchButtonArea.visible = true
                        if(!searchButtonMouseArea.containsMouse) {
                            handHide = false
                        }
                    }
                }

                Item {
                    id: searchEntryArea
                    width: 0
                    height: parent.height
                    clip: true
                    anchors.right: parent.right

                    DTextInput {
                        id: searchInput
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        text: ""
                        onTextChanged: {
                            searchResultListView.keyword = text
                        }

                        Keys.onEscapePressed: {
                            text = ""
                            if(!hideSearchEntyArea.running){
                                hideSearchEntyArea.start()
                            }
                        }
                    }

                    DOpacityImageButton {
                        id: clearButton
                        anchors.right: parent.right
                        anchors.rightMargin: 2
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 1
                        source: "images/clear.png"

                        onClicked: {
                            if(searchInput.text == "" && !hideSearchEntyArea.running){
                                hideSearchEntyArea.start()
                            }
                            else{
                                searchInput.text = ""
                            }
                        }
                    }
                }

                Image {
                    id: searchButtonArea
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    source: "images/search_normal.png"

                    MouseArea {
                        id: searchButtonMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            if(!showSearchEntyArea.running && !handHide){
                                showSearchEntyArea.start()
                            }
                        }
                        onExited: {
                            handHide = false
                        }
                    }
                }

            }
        }

        DSeparatorHorizontal {}

        DBaseLine{
            id: secondTitle
            property string text: dsTr("Shortcuts")
            leftLoader.sourceComponent: DssH2 {
                text: secondTitle.text
                font.bold: true
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.9)
            }
        }

        DSeparatorHorizontal {}
    }

    Column {
        id: keybindingListColumn
        anchors.top: keybindingTitleColumn.bottom
        width: parent.width
        height: childrenRect.height
        clip: true

        Rectangle {
            height: childrenRect.height
            width: parent.width
            color: dconstants.contentBgColor

            ListView {
                id: searchResultListView
                width: parent.width
                height: {
                    if(default_applications.height - keybindingTitleColumn.height - 2 < searchResultListView.count * 30){
                        return default_applications.height - keybindingTitleColumn.height - 2
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
                        //allCategoriesArea.visible = false
                        var results = pinyinId.SearchKeys(keyword, searchMd5)
                        for(var i in results){
                            resultKeyBindings.push(searchObject.keyBindings[results[i]])
                        }
                        if(results.length == 0){
                            secondTitle.text = "<font color='red'>No result</font>" 
                        }
                        else if(results.length == 1){
                            secondTitle.text = "<font color='red'>1</font> result"
                        }
                        else{
                            secondTitle.text = "<font color='red'>" + results.length  + "</font> results"
                        }
                    }
                    else{
                        //allCategoriesArea.visible = true
                        secondTitle.text = dsTr("Shortcuts")
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

        Item {
            id: allCategoriesArea
            width: parent.width
            height: content.height
            clip: true

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

                ObjectModelItem {
                    id: customItem
                    property string name: dsTr("Custom")
                    property var myIndex: ObjectModel.index
                    property var keyBindings: bindManagerId.customList
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

                DBaseLine{
                    height: 40
                    rightMargin: 10
                    rightLoader.sourceComponent: DTextButton{
                        text: dsTr("Reset")
                    }
                }
            }
        }

    }
}
