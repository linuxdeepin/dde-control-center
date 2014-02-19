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
    property int contentHeight: 60
    property int sliderWidth: 178

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
    property var userLayouts: ["us", "gb"]

    function isInUserLayouts(key){
        for(var i=0; i<userLayouts.length; i++){
            if(userLayouts[i] == key){
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

        DBaseLine {
            height: contentHeight
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 16
                DssH2 {
                    text: dsTr("Repeat Delay")
                    anchors.verticalCenter: parent.verticalCenter
                }
                DSliderRect {
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
        }

        DBaseLine {
            //color: dconstants.contentBgColor
            height: contentHeight
            leftMargin: contentLeftMargin

            rightLoader.sourceComponent: Row {
                spacing: 16

                DssH2 {
                    text: dsTr("Repeat Interval")
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSliderRect {
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
        }

        DBaseLine {
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 16

                DssH2 {
                    text: dsTr(" Repeat Test")
                }

                DTextInput {
                    id: testRepeatIntervalInput
                    width: sliderWidth
                    //text: dsTr("Test repeat interval")
                }
            }
        }

        //DSeparatorHorizontal {}

        DBaseLine {
            height: contentHeight
            rightLoader.sourceComponent: Row {
                spacing: 16

                DssH2 {
                    text: dsTr("Cusor Blink")
                    anchors.verticalCenter: parent.verticalCenter
                }

                DSliderRect{
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
        }

        DBaseLine {
            leftMargin: contentLeftMargin
            rightLoader.sourceComponent: Row {
                spacing: 16

                DssH2 {
                    text: dsTr("Blink Show")
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item {
                    width: sliderWidth
                    height: parent.height

                    Rectangle {
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
                                keyboardModule.userLayouts.push(selectedKeys[i])
                            }
                        }
                    }
                    keyboardLayoutArea.currentActionStateName = currentActionStateName
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

                property string selectLayoutId: keyboardID.keyboardLayout ? keyboardID.keyboardLayout[0] : "us"
                property bool inDeleteAction: keyboardLayoutArea.currentActionStateName == "deleteButton"

                function switchLayout(id){
                    keyboardID.keyboardLayout = [id]
                }

                function deleteLayout(id){
                    print("==> deleteButton")
                }

                model: {
                    var myModel = listModelComponent.createObject(layoutList, {})
                    var userKeyboardLayouts = keyboardModule.userLayouts
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
                property string currentSelectedIndex: "A"

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
                height: keyboardModule.height - 360
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

                model: {
                    var myModel = listModelComponent.createObject(addLayoutList, {})
                    if(searchMd5){
                        var search_result = searchId.SearchKeysByFirstLetter(addLayoutIndex.currentSelectedIndex, keyboardModule.searchMd5)
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
                    return myModel
                }

                delegate: AddLayoutItem {}
                DScrollBar { flickable: addLayoutList }
            }
        } // End of addLayoutArea

        DSeparatorHorizontal {}
    }
}
