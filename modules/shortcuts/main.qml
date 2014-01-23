import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQml.Models 2.1
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Item {
    id: default_applications
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22

    property var grabManagerId: GrabKey {}
    property var bindManagerId: KeyBinding {}
    property int currentShortcutId: -1
    property int expandItemIndex: -1
    property var conflictInvalid: bindManagerId.conflictInvalid
    property var conflictValid: bindManagerId.conflictValid

    Column {
        id: defaultApplicationsColumn
        anchors.top: parent.top
        width: parent.width

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
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Shortcuts")
                font.bold: true
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.9)
            }
        }

        DSeparatorHorizontal {}

        Item {
            id: allArea
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
                    model: allArea.expandItems
                }
            }
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
