import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import DBus.Com.Deepin.Api.Search 1.0
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
    property var searchId: Search {}
    property var xkeyboardLocale: DLocale { domain: "xkeyboard-config" }

    property var allLayoutMapL10n: new Object()

    property string searchMd5: ""

    function getAllLayoutMapL10n(){
        var layoutMap = dbusKeyboard.LayoutList()
        layoutMap[";"] = layoutMap["us;"]
        return layoutMap
    }

    function isInUserLayouts(key){
        for(var i=0; i<dbusKeyboard.userLayoutList.length; i++){
            if(dbusKeyboard.userLayoutList[i] == key){
                return true
            }
        }
        return false
    }

    Component.onCompleted:{
        keyboardModule.allLayoutMapL10n = getAllLayoutMapL10n()
        keyboardModule.searchMd5 = searchId.NewTrieWithString(
            keyboardModule.allLayoutMapL10n, "deepin-system-settings-keyboard-layouts")
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
            text:dsTr("Keyboard")
            rightLoader.sourceComponent: ResetButton {
                onClicked: dbusKeyboard.Reset()
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
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
            centerPadding: centerPadding
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
            centerPadding: centerPadding
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
                    windowView.setCursorFlashTime(parseInt(value))
                }

                Component.onCompleted: {
                    addRuler(100, dsTr("Fast"))
                    addRuler(2500, dsTr("Slow"))
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
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
    }

    Column {
        id: keyboardLayoutArea
        width: parent.width
        height: childrenRect.height
        anchors.top: contentColumn.bottom
        anchors.left: contentColumn.left

        property string currentActionStateName: ""

        DBaseLine {
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
                        addLayoutIndex.currentSelectedIndex = "A"
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
                property string currentSelectedIndex: ""

                onCurrentSelectedIndexChanged: {
                    addLayoutList.rebuildModel(currentSelectedIndex)
                }

                Row{
                    spacing: 3
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater{
                        model: ["A", "B", "C", "D", "E", "F", "G",
                            "H", "I", "J", "K", "L", "M", "N",
                            "O", "P", "Q", "R", "S", "T",
                            "U", "V", "W", "X", "Y", "Z"
                        ]

                        DLabel{
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData
                            font.pixelSize: addLayoutIndex.currentSelectedIndex == modelData ? 18 : 13
                            color: {
                                if (addLayoutIndex.currentSelectedIndex == modelData | hovered){
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
                                onClicked: addLayoutIndex.currentSelectedIndex = modelData
                            }
                        }
                    }
                }
            } // End of addLayoutIndex

            DSeparatorHorizontal {}

            ListView {
                id: addLayoutList
                height: {
                    var listHeight = addLayoutList.model.count * 28
                    if(listHeight > keyboardModule.height - 278 - 64){
                        return keyboardModule.height - 278 - 64
                    }
                    else{
                        return listHeight
                    }
                }
                width: parent.width
                clip: true

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

                function rebuildModel(selectedIndex){
                    addLayoutList.model.clear()
                    if(keyboardModule.searchMd5 && selectedIndex){
                        var search_result = searchId.SearchKeysByFirstLetter(selectedIndex, keyboardModule.searchMd5)
                        for (var i=0; i<search_result.length; i++){
                            var id = search_result[i]
                            if(!keyboardModule.isInUserLayouts(id)){
                                addLayoutList.model.append({
                                    "label": allLayoutMapL10n[id],
                                    "item_id": id
                                })
                            }
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

        DBaseLine {}
        DSeparatorHorizontal {}
        DBaseLine {
            leftLoader.sourceComponent: DssH2 { text: dsTr("Caps-Lock key press tip") }
            rightLoader.sourceComponent: DSwitchButton {
                checked: dbusKeyboard.capslockToggle

                onClicked: {
                    if(pressed){
                        dbusKeyboard.capslockToggle = checked
                    }
                }
            }
        }
        DSeparatorHorizontal {}
    }
}
