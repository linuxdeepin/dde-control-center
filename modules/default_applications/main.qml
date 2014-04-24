import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import DBus.Com.Deepin.Daemon.Mime 1.0
import Deepin.Widgets 1.0
import "../shared/"

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

            rightLoader.sourceComponent: ResetButton {
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
                    "setTypeGroup": [
                        "x-scheme-handler/ftp",
                        "x-scheme-handler/http",
                        "x-scheme-handler/https",
                        "text/html",
                        "text/xml",
                        "text/xhtml_xml",
                        "text/xhtml+xml"
                    ]
                },
                {
                    "name": dsTr("Mail"),
                    "icon": "images/mail.png",
                    "defaultGetType": "x-scheme-handler/mailto",
                    "setTypeGroup": [
                        "x-scheme-handler/mailto",
                        "message/rfc822",
                        "application/x-extension-eml",
                        "application/x-xpinstall"
                    ]
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
                    "setTypeGroup": [
                        "audio/mp3",
                        "audio/x-mp3",
                        "audio/mpeg3",
                        "audio/x-mpeg-3",
                        "audio/mpeg",
                        "audio/x-mpeg",
                        "audio/flac",
                        "audio/x-flac",
                        "application/x-flac",
                        "audio/ape",
                        "audio/x-ape",
                        "application/x-ape",
                        "audio/ogg",
                        "audio/x-ogg",
                        "audio/musepack",
                        "application/musepack",
                        "audio/x-musepack",
                        "application/x-musepack",
                        "audio/mpc",
                        "audio/x-mpc",
                        "audio/vorbis",
                        "audio/x-vorbis",
                        "audio/x-wav",
                        "audio/x-ms-wma"
                    ]
                },
                {
                    "name": dsTr("Video"),
                    "icon": "images/video.png",
                    "defaultGetType": "video/mp4",
                    "setTypeGroup": [
                        "audio/mp4",
                        "video/mp4",
                        "audio/x-matroska",
                        "video/x-matroska",
                        "application/x-matroska",
                        "video/avi",
                        "video/msvideo",
                        "video/x-msvideo",
                        "video/ogg",
                        "application/ogg",
                        "application/x-ogg",
                        "video/3gpp",
                        "video/3gpp2",
                        "video/flv",
                        "video/x-flv",
                        "video/x-flic",
                        "video/mpeg",
                        "video/x-mpeg",
                        "video/x-ogm",
                        "application/x-shockwave-flash",
                        "video/x-theora",
                        "video/quicktime",
                        "video/x-ms-asf",
                        "video/x-ms-wmv"
                    ]
                },
                {
                    "name": dsTr("Picture Viewer"),
                    "icon": "images/picture.png",
                    "defaultGetType": "image/jpeg",
                    "setTypeGroup": [
                        "image/jpeg",
                        "image/pjpeg",
                        "image/bmp",
                        "image/x-bmp",
                        "image/png",
                        "image/x-png",
                        "image/tiff",
                        "image/svg+xml",
                        "image/x-xbitmap",
                        "image/x-xpixmap",
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

                        property string defaultDesktopName: mediaMountId.DefaultMediaAppByMime(componentData.contentType)[0]

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
                                    mediaMountId.SetMediaAppByMime(componentData.contentType, desktopName)
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
                            var datas = mediaMountId.MediaAppListByMime(componentData.contentType)
                            for (var i=0;i<datas.length;i++){
                                autoPlayModel.append({
                                    "label": datas[i][1],
                                    "desktop_name": datas[i][0]
                                })
                            }
                            height = autoPlayModel.count * 28
                        }
                    }

                }
            } // End of MultiExpandArea
        }

        DSeparatorHorizontal {}
    }
}
