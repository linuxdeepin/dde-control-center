import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.KeyBinding 1.0
import "../widgets"
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
                //print(keybindingId.GetSystemList())
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
                    "keyBindings": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 645, 670, 671]
                },
                {
                    "name": dsTr("Sound and Media"),
                    //"icon": "images/mail.png",
                    "keyBindings": [302, 305, 306, 334, 314, 319, 318, 326, 329, 330, 331, 320, 315]
                },
                {
                    "name": dsTr("Window"),
                    //"icon": "images/document.png",
                    "keyBindings": [600, 601, 602, 603, 611, 614, 676, 677]
                },
                {
                    "name": dsTr("Workspace"),
                    //"icon": "images/music.png",
                    "keyBindings": [654, 655, 656, 657, 666, 667, 668, 669, 900, 901, 902, 903, 636, 637, 638, 639, 800, 801]
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
                                "shortcutId": shortcutId
                            })
                        }
                    }
                }
            }

        }
    }
}
