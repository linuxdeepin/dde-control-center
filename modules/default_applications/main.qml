/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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

    signal resetEmit

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: modulesId.moduleLocaleNames["default_applications"]

            rightLoader.sourceComponent: ResetButton {
                onClicked: {
                    defaultAppsId.Reset()
                    mediaMountId.Reset()
                    default_applications.resetEmit()
                }
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
			"application/vnd.rn-realmedia",
                        "video/x-ms-wmv"
                    ]
                },
                {
                    "name": dsTr("Picture"),
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

                    Connections {
                        target: default_applications
                        onResetEmit: {
                            defaultDesktopName = defaultAppsId.DefaultAppViaType(componentData.defaultGetType)[0]
                        }
                    }

                    model: ListModel {}
                    delegate: SelectItem {
                        selectItemId: defaultAppListview.defaultDesktopName
                        totalItemNumber: defaultAppListview.count

                        onSelectAction: {
                            defaultAppListview.defaultDesktopName = itemId
                            for(var i=0;i<setTypeGroup.length;i++){
                                defaultAppsId.SetDefaultAppViaType(setTypeGroup[i], itemId)
                            }
                        }
                    }
                    focus: true
                    interactive: true

                    Component.onCompleted: {
                        var datas = defaultAppsId.AppsListViaType(componentData.defaultGetType)
                        height = datas.length * 28
                        for (var i=0;i<datas.length;i++){
                            model.append({
                                "item_name": datas[i][1],
                                "item_id": datas[i][0]
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
                text: dsTr("AutoPlay")
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

                        Connections {
                            target: default_applications
                            onResetEmit: {
                                defaultDesktopName = mediaMountId.DefaultMediaAppByMime(componentData.contentType)[0]
                            }
                        }

                        model: ListModel {id: autoPlayModel}
                        delegate: SelectItem {
                            selectItemId: autoPlayAppListView.defaultDesktopName
                            totalItemNumber: autoPlayAppListView.count

                            onSelectAction: {
                                autoPlayAppListView.defaultDesktopName = itemId
                                mediaMountId.SetMediaAppByMime(componentData.contentType, itemId)
                            }
                        }
                        focus: true
                        interactive: true

                        Component.onCompleted: {
                            var datas = mediaMountId.MediaAppListByMime(componentData.contentType)
                            for (var i=0;i<datas.length;i++){
                                autoPlayModel.append({
                                    "item_name": datas[i][1],
                                    "item_id": datas[i][0]
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
