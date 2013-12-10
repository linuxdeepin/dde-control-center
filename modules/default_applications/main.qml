import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.DefaultApps 1.0
import "../widgets"

Item {
    id: default_applications
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property var defaultApps: DefaultApps {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Default Applications")
        }

        DSeparatorHorizontal {}

        DBaseHeader {
            title.sourceComponent: DssH2 {
                text: dsTr("Default Applications")
            }
        }

        DSeparatorHorizontal {}

        MultiExpandArea {
            expandItems: [
                { "name": dsTr("Browser"), "icon": "images/network.png" },
                { "name": dsTr("Mail"), "icon": "images/mail.png" },
                { "name": dsTr("Text"), "icon": "images/document.png" },
                { "name": dsTr("Music"), "icon": "images/music.png" },
                { "name": dsTr("Video"), "icon": "images/video.png" },
                { "name": dsTr("Picture Viewer"), "icon": "images/picture.png" },
                { "name": dsTr("Terminal"), "icon": "images/terminal.png" }
            ]

            modelComponent: Component {
                Column {
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                }
            }

        }

        DSwitchButtonHeader {
            text: dsTr("Auto Play")

            onClicked: {
                print(active)
            }
        }

        DSeparatorHorizontal {}

        MultiExpandArea {
            expandItems: [
                { "name": dsTr("CD Audio"), "icon": "images/cd.png" },
                { "name": dsTr("DVD Video"), "icon": "images/dvd.png" },
                { "name": dsTr("Music Player"), "icon": "images/music_player.png" },
                { "name": dsTr("Camera"), "icon": "images/camera.png" },
                { "name": dsTr("Software"), "icon": "images/application.png" }
            ]

            modelComponent: Component {
                Column {
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                    DLabel {text: "test"}
                }
            }

        }
    }
}
