import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import "../widgets"

Item {
    id: default_applications
    anchors.fill: parent

    property var dconstants: DConstants {}
    property int contentLeftMargin: 22
    property int contentHeight: 60

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Shortcuts")
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
                    "icon": "images/network.png",
                    "defaultGetType": "x-scheme-handler/http", 
                    "setTypeGroup": ["x-scheme-handler/http"]
                },
                {
                    "name": dsTr("Sound and Media"),
                    "icon": "images/mail.png",
                    "defaultGetType": "x-scheme-handler/mailto",
                    "setTypeGroup": ["x-scheme-handler/mailto"]
                },
                {
                    "name": dsTr("Window"),
                    "icon": "images/document.png",
                    "defaultGetType": "text/plain",
                    "setTypeGroup": ["text/plain"]
                },
                {
                    "name": dsTr("Workspace"),
                    "icon": "images/music.png",
                    "defaultGetType": "audio/mpeg",
                    "setTypeGroup": ["audio/mpeg"]
                },
                {
                    "name": dsTr("Custom"),
                    "icon": "images/video.png",
                    "defaultGetType": "video/mp4",
                    "setTypeGroup": ["video/mp4"]
                }
            ]

            modelComponent: Component {

                ListView {
                    height: 60
                    width: parent.width
                    focus: true

                    model: ListModel {
                        ListElement { name: "Bob" }
                        ListElement { name: "John" }
                        ListElement { name: "Michael" }
                    }

                    delegate: FocusScope {
                            width: parent.width; height: 28
                            //x:childrenRect.x; y: childrenRect.y
                            TextInput {
                                anchors.fill: parent
                                focus: true
                                text: name
                                Keys.onReturnPressed: console.log(name)
                                cursorVisible: false
                                readOnly: true
                                color: activeFocus ? "blue" : "white"

                                property string oldShortcut

                                Keys.onReleased: {
                                    if(event.key == Qt.Key_Escape){
                                        parent.focus = false
                                    }
                                }

                                onActiveFocusChanged: {
                                    if (activeFocus){
                                        print(parent.width)
                                        oldShortcut = text
                                        text = "get"
                                    }
                                    else {
                                        text = oldShortcut
                                    }
                                }
                            }
                    }
                }

            }

        }
    }
}
