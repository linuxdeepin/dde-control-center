import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.Mime 1.0
import Deepin.Widgets 1.0

Item {
    id: default_applications
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int contentHeight: 60
    property var defaultAppsId: DefaultApps {}
    property var mediaMountId: MediaMount {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Default Applications")

            rightLoader.sourceComponent: DTextButton {
                text: dsTr("Reset")
            }
        }

        DSeparatorHorizontal {}

        DBaseLine{
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Default Applications")
                font.bold: true
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.9)
            }
        }

        DSeparatorHorizontal {}

        MultiExpandArea {
            expandItems: [
                {
                    "name": dsTr("Browser"),
                    "icon": "images/network.png",
                    "defaultGetType": "x-scheme-handler/http", 
                    "setTypeGroup": ["x-scheme-handler/http"]
                },
                {
                    "name": dsTr("Mail"),
                    "icon": "images/mail.png",
                    "defaultGetType": "x-scheme-handler/mailto",
                    "setTypeGroup": ["x-scheme-handler/mailto"]
                },
                {
                    "name": dsTr("Text"),
                    "icon": "images/document.png",
                    "defaultGetType": "text/plain",
                    "setTypeGroup": ["text/plain"]
                },
                {
                    "name": dsTr("Music"),
                    "icon": "images/music.png",
                    "defaultGetType": "audio/mpeg",
                    "setTypeGroup": ["audio/mpeg"]
                },
                {
                    "name": dsTr("Video"),
                    "icon": "images/video.png",
                    "defaultGetType": "video/mp4",
                    "setTypeGroup": ["video/mp4"]
                },
                {
                    "name": dsTr("Picture Viewer"),
                    "icon": "images/picture.png",
                    "defaultGetType": "image/jpeg",
                    "setTypeGroup": [
                        "image/jpeg",
                        "image/bmp",
                        "image/png",
                        "image/tiff",
                        "image/gif"
                    ]
                },
                {
                    "name": dsTr("Terminal"),
                    "icon": "images/terminal.png",
                    "defaultGetType": "terminal",
                    "setTypeGroup": ["terminal"]
                }
            ]

            modelComponent: Component {

                ListView {
                    id: defaultAppListview
                    width: parent.width

                    property string defaultDesktopName: defaultAppsId.DefaultAppViaType(componentData.defaultGetType)[0]
                    property var setTypeGroup: componentData.setTypeGroup

                    model: ListModel {}
                    delegate: Item {
                        width: parent.width
                        height: 28
                        anchors.left: parent.left
                        anchors.leftMargin: 25

                        property string desktopName: desktop_name
                        
                        Row {
                            spacing: 5
                            anchors.verticalCenter: parent.verticalCenter
                            
                            Image {
                                id: nameImage
                                anchors.verticalCenter: parent.verticalCenter
                                source: "images/select.png"
                                opacity: defaultAppListview.defaultDesktopName == desktopName ? 1 : 0
                            }
                            
                            DssH3 {
                                id: nameText
                                anchors.verticalCenter: parent.verticalCenter
                                text: label 
                                color: defaultAppListview.defaultDesktopName == desktopName ? "#009EFF" : "#fff"
                                font.pixelSize: 12
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            
                            onEntered: {
                                defaultAppListview.currentIndex = index
                            }
                            
                            onClicked: {
                                defaultAppListview.defaultDesktopName = desktopName
                                for(var i=0;i<setTypeGroup.length;i++){
                                    defaultAppsId.SetDefaultAppViaType(setTypeGroup[i], desktopName)
                                }
                            }
                        }
                    }

                    highlight: Rectangle {
                        width: parent.width
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        height: 28
                        color: "#0D0D0D"
                        radius: 4
                    }
                    highlightMoveDuration: 200
                    focus: true
                    interactive: true

                    Component.onCompleted: {
                        var datas = defaultAppsId.AppsListViaType(componentData.defaultGetType)
                        height = datas.length * 28
                        for (var i=0;i<datas.length;i++){
                            model.append({
                                "label": datas[i][1],
                                "desktop_name": datas[i][0]
                            })
                        }
                    }
                } //end ListView
            }

        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: autoPlayExpand
            expanded: header.item.active
            header.sourceComponent: DSwitchButtonHeader {
                text: dsTr("Auto Play")
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.9)
                font.bold: true
                rightMargin: 13
                active: mediaMountId.autoMountOpen

                onClicked: {
                    mediaMountId.autoMountOpen = active
                }
            }

            content.sourceComponent: MultiExpandArea {
                expandItems: [
                    { 
                        "name": dsTr("CD Audio"),
                        "icon": "images/cd.png",
                        "contentType": "x-content/audio-cdda"
                    },
                    {
                        "name": dsTr("DVD Video"),
                        "icon": "images/dvd.png",
                        "contentType": "x-content/video-dvd"
                    },
                    {
                        "name": dsTr("Music Player"),
                        "icon": "images/music_player.png",
                        "contentType": "x-content/audio-player"
                    },
                    {
                        "name": dsTr("Camera"),
                        "icon": "images/camera.png",
                        "contentType": "x-content/image-dcf"
                    },
                    {
                        "name": dsTr("Software"),
                        "icon": "images/application.png",
                        "contentType": "x-content/unix-software"
                    }
                ]

                modelComponent: Component {

                    ListView {
                        id: autoPlayAppListView
                        width: parent.width

                        property string defaultItemId: ""

                        model: ListModel {id: autoPlayModel}
                        delegate: Item {
                            width: parent.width
                            height: 28
                            anchors.left: parent.left
                            anchors.leftMargin: 25

                            property string desktopName: desktop_name
                            
                            Row {
                                spacing: 5
                                anchors.verticalCenter: parent.verticalCenter
                                
                                Image {
                                    id: nameImage
                                    anchors.verticalCenter: parent.verticalCenter
                                    source: "images/select.png"
                                    opacity: autoPlayAppListView.defaultDesktopName == desktopName ? 1 : 0
                                }
                                
                                DssH3 {
                                    id: nameText
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: label 
                                    color: autoPlayAppListView.defaultDesktopName == desktopName ? "#009EFF" : "#fff"
                                    font.pixelSize: 12
                                }
                            }
                            
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                
                                onEntered: {
                                    autoPlayAppListView.currentIndex = index
                                }
                                
                                onClicked: {
                                    autoPlayAppListView.defaultDesktopName = desktopName
                                }
                            }
                        }

                        highlight: Rectangle {
                            width: parent.width
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            height: 28
                            color: "#0D0D0D"
                            radius: 4
                        }
                        highlightMoveDuration: 200
                        focus: true
                        interactive: true

                        Component.onCompleted: {
                            var datas = defaultAppsId.AppsListViaType(componentData.contentType)
                            for (var i=0;i<datas.length;i++){
                                autoPlayModel.append({
                                    "label": datas[i][1],
                                    "desktop_name": datas[i][0]
                                })
                            }
                            autoPlayModel.append({
                                "label": "Nothing",
                                "desktop_name": "nothing"
                            })
                            autoPlayModel.append({
                                "label": "Open folder",
                                "desktop_name": "folder"
                            })
                            height = autoPlayModel.count * 28
                        }
                    }

                }
            } // End of MultiExpandArea
        }

        DSeparatorHorizontal {}
    }
}
