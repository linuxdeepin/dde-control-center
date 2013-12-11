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
    property var defaultAppsId: DefaultApps {}

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
                font.bold: true
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.9)
            }
        }

        DSeparatorHorizontal {}

        MultiExpandArea {
            //self.http_content_type = ""
            //self.mail_content_type = ""
            //self.calendar_content_type = "text/calendar"
            //self.editor_content_type = ""
            //self.audio_content_type = ""
            //self.video_content_type = ""
            //self.photo_content_type = ""
            //self.rough_types = ["audio", "video", "image"]
            //self.rough_type_lists = {
                    //"image": [
                        //"image/jpeg",
                        //"image/bmp",
                        //"image/png",
                        //"image/tiff",
                        //"image/gif",
                        //],
                    //}

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
                //{
                    //"name": dsTr("Terminal"),
                    //"icon": "images/terminal.png"
                    //"defaultGetType": ""
                    //"setTypeGroup": [""]
                //}
            ]

            modelComponent: Component {

                ListView {
                    id: listview
                    width: parent.width
                    height: 28 * model.length

                    property string currentName: ""

                    model: ListModel {}
                    delegate: Item {
                        width: parent.width
                        height: 28
                        anchors.left: parent.left
                        anchors.leftMargin: 25
                        
                        Row {
                            spacing: 5
                            anchors.verticalCenter: parent.verticalCenter
                            
                            Image {
                                id: nameImage
                                anchors.verticalCenter: parent.verticalCenter
                                source: "images/select.png"
                                opacity: selected ? 1 : 0
                            }
                            
                            DssH3 {
                                id: nameText
                                anchors.verticalCenter: parent.verticalCenter
                                text: label 
                                color: selected ? "#009EFF" : "#fff"
                                font.pixelSize: 12
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            
                            onEntered: {
                                listview.currentIndex = index
                            }
                            
                            onClicked: {
                            }
                        }
                    }

                    highlight: Rectangle {
                        width: parent.width
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        height: 24
                        color: "#0D0D0D"
                        radius: 4
                    }
                    highlightMoveDuration: 200
                    focus: true
                    interactive: true

                    Component.onCompleted: {
                        //model.append({"label": "正常模式", "selected": dde_desktop.dockMode == "default"})
                        //model.append({"label": "自动隐藏", "selected": dde_desktop.dockMode == "autohide"})
                        //model.append({"label": "一直隐藏", "selected": dde_desktop.dockMode == "keephidden"})
                        var datas = defaultAppsId.GetAppsListViaType(componentData.defaultGetType)
                        listview.height = datas.length * 28
                        for (var i=0;i<datas.length;i++){
                            model.append({
                                "label": datas[i][1],
                                "selected": datas[i][1] == defaultAppsId.GetDefaultAppViaType(componentData.defaultGetType, false)[1]
                            })
                        }
                    }
                }
            }

        }

        DSwitchButtonHeader {
            text: dsTr("Auto Play")
            style: Text.Raised
            styleColor: Qt.rgba(0, 0, 0, 0.9)
            font.bold: true
            rightMargin: 13

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
