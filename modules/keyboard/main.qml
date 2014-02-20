import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Locale 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.InputDevices 1.0
import DBus.Com.Deepin.Api.Search 1.0

Item {
    id: keyboardModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 40
    property int sliderWidth: 178

    property int titleWidth: 100
    property int centerPadding: 20

    property var dconstants: DConstants {}
    property var keyboardID: Keyboard {}
    property var searchId: Search {}
    property var xkeyboardLocale: DLocale { domain: "xkeyboard-config" }

    property var allLayoutMapL10n: {
        var layoutMap = keyboardID.LayoutList()
        for(var key in layoutMap){
            layoutMap[key] = xkeyboardLocale.dsTr(layoutMap[key])
        }
        return layoutMap
    }

    property string searchMd5: searchId.NewTrieWithString(allLayoutMapL10n, "deepin-system-settings-keyboard-layouts")

    function isInUserLayouts(key){
        for(var i=0; i<keyboardID.userLayoutList.length; i++){
            if(keyboardID.userLayoutList[i] == key){
                return true
            }
        }
        return false
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
            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Reset")
            }
        }

        DSeparatorHorizontal {}

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Repeat Delay")
            content.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Long")
                rightLabel: dsTr("Short")

                value: (2000 - keyboardID.repeatDelay)/1900
                realValue: keyboardID.repeatDelay
                onValueChanged: {
                    if (keyboardID.repeatDelay != (2000 - 1900 * value)){
                        keyboardID.repeatDelay = (2000 - 1900 * value)
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Repeat Interval")

            content.sourceComponent: DSliderRect {
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (2000 - keyboardID.repeatSpeed)/1980
                realValue: keyboardID.repeatSpeed
                onValueChanged: {
                    if (keyboardID.repeatSpeed != (2000 - 1980 * value)){
                        keyboardID.repeatSpeed = 2000 - 1980 * value
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Repeat Test")

            content.sourceComponent: DTextInput {
                id: testRepeatIntervalInput
                width: sliderWidth
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Cursor Blink")

            content.sourceComponent: DSliderRect{
                id: cursorBlinkSlider
                width: sliderWidth
                leftLabel: dsTr("Slow")
                rightLabel: dsTr("Fast")

                value: (2500 - keyboardID.cursorBlink)/2400
                realValue: keyboardID.cursorBlink
                onValueChanged: {
                    if (keyboardID.cursorBlink != (2500 - 2400 * value)){
                        keyboardID.cursorBlink = 2500 - 2400 * value
                    }
                }
            }
        }

        DCenterLine {
            height: contentHeight
            centerPadding: centerPadding
            leftWidth: titleWidth
            title.text: dsTr("Cursor Blink Show")

            content.sourceComponent: Rectangle {
                width: 1
                height: 14
                color: "white"
                anchors.verticalCenter: parent.verticalCenter

                Timer {
                    running: true
                    interval: keyboardID.cursorBlink/2
                    repeat: true
                    onTriggered: parent.visible = !parent.visible
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
                                keyboardID.AddUserLayout(selectedKeys[i])
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

                property string selectLayoutId: keyboardID.currentLayout ? keyboardID.currentLayout : "us;"
                property bool inDeleteAction: keyboardLayoutArea.currentActionStateName == "deleteButton"

                function switchLayout(id){
                    keyboardID.currentLayout = id
                }

                function deleteLayout(id){
                    keyboardID.DeleteUserLayout(id)
                }

                model: {
                    var myModel = listModelComponent.createObject(layoutList, {})
                    var userKeyboardLayouts = keyboardID.userLayoutList
                    for (var i=0; i<userKeyboardLayouts.length; i++){
                        var id = userKeyboardLayouts[i]
                        myModel.append({
                            "label": allLayoutMapL10n[id],
                            "item_id": id
                        })
                    }
                    return myModel
                }

                delegate: LayoutItem {}
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
                                    return "#009EFF"
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
                    var listHeight = myModel.count * 28
                    if(listHeight > keyboardModule.height - 360){
                        return keyboardModule.height - 360
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
                    myModel.clear()
                    if(keyboardModule.searchMd5 && selectedIndex){
                        var search_result = searchId.SearchKeysByFirstLetter(selectedIndex, keyboardModule.searchMd5)
                        for (var i=0; i<search_result.length; i++){
                            var id = search_result[i]
                            if(!keyboardModule.isInUserLayouts(id)){
                                myModel.append({
                                    "label": allLayoutMapL10n[id],
                                    "item_id": id
                                })
                            }
                        }
                    }
                }

                model: ListModel { id: myModel }

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
    }
}
