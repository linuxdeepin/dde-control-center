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

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import DBus.Com.Deepin.Daemon.Search 1.0
import "../shared/"

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 40
    property int sliderWidth: 178

    property int titleWidth: 100
    property int centerPadding: 20

    property var dconstants: DConstants {}
    property var dbusKeyboard: Keyboard {}
    property var dbusSearch: Search {}

    property var allLayoutMapL10n: new Object()

    property string searchMd5: ""

    property var layoutValueToKey: null

    function initSearchSequence() {
        var layoutMap = dbusKeyboard.LayoutList()
        layoutValueToKey = new Object()
        for(var key in layoutMap){
            var value = layoutMap[key]
            layoutValueToKey[value] = key
        }
    }

    function isInUserLayouts(key){
        var userLayoutList = dbusKeyboard.userLayoutList
        for(var i=0; i<userLayoutList.length; i++){
            if(userLayoutList[i] == key){
                return true
            }
        }
        return false
    }

    Component.onCompleted:{
        initSearchSequence()
        keyboardModule.allLayoutMapL10n = dbusKeyboard.LayoutList()

        var valueList = Object.keys(layoutValueToKey).sort()
        var retList = dbusSearch.NewSearchWithStrList(valueList)
        keyboardModule.searchMd5 = retList[0]
        layoutList.reloadLayout()
    }

    Connections {
        target: dbusKeyboard
        onUserLayoutListChanged: {
            layoutList.reloadLayout()
        }
    }

    Component {
        id: listModelComponent
        ListModel {}
    }

    Column {
        id: contentColumn
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height

        DssTitle {
            text: modulesId.moduleLocaleNames["keyboard"]
            rightLoader.sourceComponent: ResetButton {
                onClicked: dbusKeyboard.Reset()
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            height: contentHeight
            centerPadding: keyboardModule.centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Repeat Delay")
            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 20
                max: 600
                init: parseInt(dbusKeyboard.repeatDelay)
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusKeyboard.repeatDelay = value
                }

                Component.onCompleted: {
                    addRuler(20, dsTr("Short"))
                    addRuler(600, dsTr("Long"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: keyboardModule.centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Repeat Rate")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 1000
                max: 200
                init: dbusKeyboard.repeatInterval
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusKeyboard.repeatInterval = value
                }

                Component.onCompleted: {
                    addRuler(200, dsTr("Fast"))
                    addRuler(1000, dsTr("Slow"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: keyboardModule.centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Cursor Blink Rate")

            content.sourceComponent: DSliderEnhanced {
                width: sliderWidth

                min: 2500
                max: 100
                init: dbusKeyboard.cursorBlink
                valueDisplayVisible: false

                onValueConfirmed:{
                    dbusKeyboard.cursorBlink = value
                    mainObject.setCursorFlashTime(parseInt(value))
                }

                Component.onCompleted: {
                    addRuler(100, dsTr("Fast"))
                    addRuler(2500, dsTr("Slow"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: keyboardModule.centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Test Area")

            content.sourceComponent: Item {
                width: sliderWidth
                height: parent.height

                TestInput {
                    id: testRepeatIntervalInput
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: sliderWidth - 16
                    cursorBlinkTime: dbusKeyboard.cursorBlink
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 { text: dsTr("Caps Lock prompt") }
            rightLoader.sourceComponent: DSwitchButton {
                checked: dbusKeyboard.capslockToggle

                onClicked: {
                    dbusKeyboard.capslockToggle = checked
                }
            }
        }

        DSeparatorHorizontal {}
    }

    Column {
        id: keyboardLayoutArea
        width: parent.width
        height: childrenRect.height
        anchors.top: contentColumn.bottom
        anchors.left: contentColumn.left

        property string currentActionStateName: ""

        DBaseLine {
            id: keyboardLayoutAreaTitleLine
            function reset(){
                rightLoader.item.currentActionStateName = ""
            }
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Keyboard Layout")
            }
            rightLoader.sourceComponent: StateButtons {
                deleteButton.visible: layoutList.count > 1
                onCurrentActionStateNameChanged: {
                    if(keyboardLayoutArea.currentActionStateName == "addButton"){
                        var selectedKeys = addLayoutList.getSelectedKeys()
                        for(var i=0; i<selectedKeys.length; i++){
                            if(!keyboardModule.isInUserLayouts(selectedKeys[i])){
                                dbusKeyboard.AddUserLayout(selectedKeys[i])
                            }
                        }
                    }
                    keyboardLayoutArea.currentActionStateName = currentActionStateName

                    if(currentActionStateName == "addButton"){
                        indexLetterListView.currentIndex = 0
                        addLayoutList.rebuildModel(addLayoutIndex.searchIndexResult["A"])
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            id: userKeyboardLayoutsArea
            color: dconstants.contentBgColor
            width: parent.width
            height: childrenRect.height
            visible: keyboardLayoutArea.currentActionStateName != "addButton"

            ListView {
                id: layoutList
                width: parent.width
                height: count * 28
                currentIndex: -1
                clip: true

                property string selectLayoutId: dbusKeyboard.currentLayout
                property bool inDeleteAction: keyboardLayoutArea.currentActionStateName == "deleteButton"

                function switchLayout(id){
                    dbusKeyboard.currentLayout = id
                }

                function deleteLayout(id){
                    dbusKeyboard.DeleteUserLayout(id)
                }

                function reloadLayout(){
                    var myModel = listModelComponent.createObject(layoutList, {})
                    var userKeyboardLayouts = dbusKeyboard.userLayoutList
                    for (var i=0; i<userKeyboardLayouts.length; i++){
                        var id = userKeyboardLayouts[i]
                        myModel.append({
                            "item_id": id,
                            "item_name": allLayoutMapL10n[id]
                        })
                    }
                    layoutList.model = myModel
                }

                delegate: SelectItem {
                    selectItemId: layoutList.selectLayoutId
                    totalItemNumber: layoutList.count
                    inDeleteAction: layoutList.inDeleteAction

                    onDeleteAction: {
                        layoutList.deleteLayout(itemId)
                    }

                    onSelectAction: {
                        layoutList.switchLayout(itemId)
                    }
                }
            }
        } // End of userKeyboardLayoutsArea

        Column {
            id: addLayoutArea
            width: parent.width
            visible: keyboardLayoutArea.currentActionStateName == "addButton"

            Rectangle {
                id: addLayoutIndex
                height: 28
                width: parent.width
                color: dconstants.bgColor

                property var searchIndexResult: null

                Component.onCompleted: {
                    searchIndexResult = new Object()
                    var alphabet = ["A", "B", "C", "D", "E", "F", "G",
                                    "H", "I", "J", "K", "L", "M", "N",
                                    "O", "P", "Q", "R", "S", "T",
                                    "U", "V", "W", "X", "Y", "Z"
                                ]
                    for(var i=0;i<alphabet.length;i++){
                        var indexLetter = alphabet[i]
                        var search_result = dbusSearch.SearchStartWithString(indexLetter, keyboardModule.searchMd5)
                        if(search_result.length > 0){
                            indexLetterListView.model.append({
                                "indexLetter": indexLetter,
                            })
                            searchIndexResult[indexLetter] = search_result
                        }
                    }
                }

                ListView {
                    id: indexLetterListView
                    width: childrenRect.width
                    height: parent.height
                    orientation: ListView.Horizontal
                    boundsBehavior: Flickable.StopAtBounds
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: ListModel{}
                    currentIndex: -1
                    delegate: DLabel{
                        anchors.verticalCenter: parent.verticalCenter
                        width: contentWidth + 4
                        text: indexLetter
                        font.pixelSize: ListView.isCurrentItem ? 18 : 13
                        color: {
                            if (ListView.isCurrentItem | hovered){
                                return dconstants.activeColor
                            }
                            return dconstants.fgColor
                        }

                        property bool hovered: false

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onReleased: containsMouse ? parent.hovered = true : parent.hovered = false
                            onClicked: parent.ListView.view.currentIndex = index
                        }
                    }
                }
            } // End of addLayoutIndex

            DSeparatorHorizontal {}

            Rectangle {
                id: addLayoutListBox
                color: dconstants.contentBgColor
                width: parent.width
                height: childrenRect.height
            }

            ListView {
                id: addLayoutList
                parent: addLayoutListBox
                height: {
                    var listHeight = addLayoutList.model.count * 28
                    return Math.min(keyboardModule.height - 278 - 64, listHeight)
                }
                width: parent.width
                clip: true

                Connections {
                    target: indexLetterListView
                    onCurrentIndexChanged: {
                        if(indexLetterListView.currentIndex != -1){
                            var dataObject = indexLetterListView.model.get(indexLetterListView.currentIndex)
                            var indexLetter = dataObject.indexLetter
                            addLayoutList.rebuildModel(addLayoutIndex.searchIndexResult[indexLetter])
                        }
                    }
                }

                function getSelectedKeys(){
                    var selectedKeys = []
                    var allChildren = contentItem.children
                    for(var i=0; i<allChildren.length; i++){
                        if(allChildren[i]["selected"]){
                            selectedKeys.push(allChildren[i].itemId)
                        }
                    }
                    return selectedKeys
                }

                function rebuildModel(search_result){
                    addLayoutList.model.clear()
                    for (var i=0; i<search_result.length; i++){
                        var id = layoutValueToKey[search_result[i]]
                        if(!keyboardModule.isInUserLayouts(id)){
                            addLayoutList.model.append({
                                "label": allLayoutMapL10n[id],
                                "item_id": id
                            })
                        }
                    }
                }

                model: ListModel {}

                delegate: AddLayoutItem {}
                DScrollBar { flickable: addLayoutList }
            }

            DBaseLine {
                height: 28
                visible: addLayoutList.model.count == 0
                color: dconstants.contentBgColor
                leftLoader.sourceComponent: DLabel{
                    text: dsTr("No result")
                }
            }
        } // End of addLayoutArea

        DSeparatorHorizontal {}

        LocaleArea {
            id: localeArea
            listAreaMaxHeight: keyboardModule.height - 278 - 94
            onExpandedChanged: {
                if(expanded){
                    keyboardLayoutAreaTitleLine.reset()
                }
            }
            Connections{
                target: keyboardLayoutArea
                onCurrentActionStateNameChanged: {
                    if(keyboardLayoutArea.currentActionStateName != ""){
                        localeArea.expanded = false
                    }
                }
            }
        }
        DSeparatorHorizontal {}

    }
}
