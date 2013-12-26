import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import Deepin.Widgets 1.0
import "./shortcuts_maps.js" as ShortcutsMap

Item {
    id: default_applications
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22
    property int contentHeight: 60

    property var keybindingId: KeyBinding {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Shortcuts")
            Component.onCompleted: {
                //print(keybindingId.keyBindingList)
                //print(default_applications.parent.y)
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

        MultiExpandArea {
            expandItems: [
                {
                    "name": dsTr("System"),
                    //"icon": "images/network.png",
                    "keyBindings": ShortcutsMap.systemBindingCategories['system']
                },
                {
                    "name": dsTr("Sound and Media"),
                    //"icon": "images/mail.png",
                    "keyBindings": ShortcutsMap.systemBindingCategories['sound and media']
                },
                {
                    "name": dsTr("Window"),
                    //"icon": "images/document.png",
                    "keyBindings": ShortcutsMap.systemBindingCategories['window']
                },
                {
                    "name": dsTr("Workspace"),
                    //"icon": "images/music.png",
                    "keyBindings": ShortcutsMap.systemBindingCategories['workspace']
                },
                {
                    "name": dsTr("Custom"),
                    //"icon": "images/video.png",
                    "keyBindings": []
                }
            ]

            modelComponent: Component {
                ListView {
                    id: lists
                    width: parent.width
                    height: lists.count * 30 > 300 ? 200 : lists.count * 30
                    focus: true
                    model: ListModel {id: keyBindingModel}
                    delegate: ShortcutInput {}

                    Component.onCompleted: {
                        for(var i=0; i<componentData.keyBindings.length; i++){
                            var shortcutId = componentData.keyBindings[i]
                            keyBindingModel.append({
                                "displayName": ShortcutsMap.currentSystemBindings[shortcutId],
                                "shortcutId": shortcutId,
                                "shortcutName": keybindingId.GetBindingAccel(shortcutId).replace(/\</g, "").replace(/\>/g, "+")
                            })
                        }
                    }
                }
            }

        }
    }
}
