import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQml.Models 2.1
import DBus.Com.Deepin.Daemon.BindManager 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Item {
    id: default_applications
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22

    property var grabManagerId: GrabManager {}
    property var bindManagerId: BindManager {}
    property int currentShortcutId: -1
    property int expandItemIndex: -1

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle { text: dsTr("Shortcuts") }

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

                    Binding {
                        target: systemItem
                        property: "keyBindings"
                        value: bindManagerId.systemList
                        when: true
                    }
                }

                ObjectModelItem {
                    id: mediaItem
                    property string name: dsTr("Sound and Media")
                    property var myIndex: ObjectModel.index
                    Binding {
                        target: mediaItem
                        property: "keyBindings"
                        value: bindManagerId.mediaList
                        when: true
                    }
                }

                ObjectModelItem {
                    id: windowItem
                    property string name: dsTr("Window")
                    property var myIndex: ObjectModel.index
                    Binding {
                        target: windowItem
                        property: "keyBindings"
                        value: bindManagerId.windowList
                        when: true
                    }
                }

                ObjectModelItem {
                    id: workspaceItem
                    property string name: dsTr("Workspace")
                    property var myIndex: ObjectModel.index
                    Binding {
                        target: workspaceItem
                        property: "keyBindings"
                        value: bindManagerId.workSpaceList
                        when: true
                    }
                }

                ObjectModelItem {
                    id: customItem
                    property string name: dsTr("Custom")
                    property var myIndex: ObjectModel.index
                    Binding {
                        target: customItem
                        property: "keyBindings"
                        value: bindManagerId.customList
                        when: true
                    }
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
    }
}
