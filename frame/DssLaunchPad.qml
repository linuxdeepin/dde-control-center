import QtQuick 2.0

Rectangle {
    anchors.fill: parent
    Component.onCompleted: {
        addModule(qsTr("System Update"), Qt.resolvedUrl("../modules/system_update/Main.qml"), "update");
        //addModule("显示",  Qt.resolvedUrl("../modules/basics/color-animation.qml"), "display");
        //addModule("桌面",  Qt.resolvedUrl("../modules/basics/property-animation.qml"), "desktop");
        //addModule("个性化", Qt.resolvedUrl("../modules/behaviors/behavior-example.qml"), "individuation");
        //addModule("声音", Qt.resolvedUrl("../modules/behaviors/wigglytext.qml"), "sound");
        //addModule("日期和时间", Qt.resolvedUrl("../modules/behaviors/tvtennis.qml"), "date_time");
        //addModule("电源", Qt.resolvedUrl("../modules/easing/easing.qml"), "power");
        //addModule("键盘", Qt.resolvedUrl("../modules/states/states.qml"), "keyboard");
        //addModule("账户", Qt.resolvedUrl("../modules/states/transitions.qml"), "account");
        //addModule("网络", Qt.resolvedUrl("../modules/pathanimation/pathanimation.qml"), "network");
        //addModule("蓝牙", Qt.resolvedUrl("../modules/pathinterpolator/pathinterpolator.qml"), "bluetooth");
        addModule(qsTr("System Information"), Qt.resolvedUrl("../modules/system_info_bak/main.qml"), "system_information");
    }

    property string dssName: "选项"
    property bool inList: false
    property int topBarWidth: 80

    color: "#252627"

    function addModule(name, url, module_id)
    {
        myModel.append({"name":name, "url":url, "module_id": module_id})
    }

    function hideExample()
    {
        moduleItem.visible = false;
    }

    ListView {
        clip: true
        delegate: DssLaunchDelegate{exampleItem: moduleItem}
        model: ListModel {id:myModel}
        anchors.fill: parent
        anchors.topMargin: bar.height
    }

    Item {
        id: moduleItem
        visible: false
        clip: true
        property url exampleUrl
        onExampleUrlChanged: visible = (exampleUrl == '' ? false : true); //Setting exampleUrl automatically shows example
        anchors.fill: parent
        anchors.topMargin: topBarWidth
        Rectangle {
            id: bg
            anchors.fill: parent
            color: "white"
        }
        MouseArea{
            anchors.fill: parent
            enabled: moduleItem.visible
            //Eats mouse events
        }
        Loader{
            focus: true
            source: moduleItem.exampleUrl
            anchors.fill: parent
        }
    }

    Rectangle {
        id: bar
        visible: true
        anchors.top: parent.top
        width: parent.width
        height: topBarWidth
        color: "#252627"

        Rectangle {
            anchors.top: parent.top
            //anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: parent.width
            height: parent.height - 36
            color: parent.color

            Image {
                id: dssImage
                width: 16
                height: 16
                source: "images/dss.png"
                anchors.top: parent.top
                anchors.topMargin: 16
                anchors.left: parent.left
                anchors.leftMargin: 14
            }

            Text {
                text: "系统设置"
                anchors.top: dssImage.top
                anchors.left: dssImage.right
                anchors.leftMargin: 5
                color: "#eaeaea"
                font.pixelSize: 11
                styleColor: "white"
            }
        }

        /** 2 line **/
        Rectangle {
            height: 1
            color: "#0b0809"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 1
            anchors.left: parent.left
            anchors.right: parent.right
        }
        Rectangle {
            height: 1
            color: "#3b3c3d"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        MouseArea{
            anchors.fill: parent
            enabled: moduleItem.visible
            //Eats mouse events
        }

        Rectangle{
            id: navigateButton
            height: 36
            width: parent.width
            color: parent.color
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2

            Image {
                id: back
                source: "images/back_normal.png"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 20

                MouseArea {
                    hoverEnabled: true
                    anchors.centerIn: parent
                    width: parent.width
                    height: parent.height
                    onClicked: {
                        moduleItem.exampleUrl = "";
                        title.text = dssName;
                    }
                    onPressed: {
                        back.source = "images/back_press.png"
                    }
                    onReleased: {
                        back.source = "images/back_normal.png"
                    }
                }
            }

            Row {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 49

                QtObject { //enumeration for topButton image
                    id: topButtonImage
                    property string headerNormal: "images/dss_top_title_header_normal.png"
                    property string headerPress: "images/dss_top_title_header_press.png"
                    property string middleNormal: "images/dss_top_title_middle_normal.png"
                    property string middlePress: "images/dss_top_title_middle_press.png"
                    property string tailNormal: "images/dss_top_title_tail_normal.png"
                    property string tailPress: "images/dss_top_title_tail_press.png"
                }

                Image{
                    id: topButtonHeader
                    source: topButtonImage.headerNormal
                }

                Image {
                    id: topButtonMiddle
                    source: topButtonImage.middleNormal
                    width: title.width + 6

                    Text{
                        id: title
                        text: dssName
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#eaeaea"
                        font.pixelSize: 13
                        styleColor: "white"
                        //style: Text.Raised
                    }

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        width: parent.width
                        height: parent.height
                        onPressed: {
                            topButtonHeader.source = topButtonImage.headerPress
                            topButtonMiddle.source = topButtonImage.middlePress
                            topButtonTail.source = topButtonImage.tailPress
                        }
                        onReleased: {
                            topButtonHeader.source = topButtonImage.headerNormal
                            topButtonMiddle.source = topButtonImage.middleNormal
                            topButtonTail.source = topButtonImage.tailNormal
                        }
                        onClicked: {
                            windowView.close()
                        }
                    }
                }

                Image{
                    id: topButtonTail
                    source: topButtonImage.tailNormal
                }

            }
        }
    }
}
