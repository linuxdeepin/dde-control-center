import QtQuick 2.0

Rectangle {
    //model is a list of {"name":"somename", "url":"file:///some/url/mainfile.qml"}
    //function used to add to model A) to enforce scheme B) to allow Qt.resolveUrl in url assignments

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
        delegate: SimpleLauncherDelegate{exampleItem: moduleItem}
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
                source: "images/back.png"
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
                        back.source = "images/back.png"
                    }
                }
            }

            Image {
                id: topButton
                source: "images/dss_top_button.png"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 50

                Text{
                    id: title
                    text: dssName
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -1
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
                        topButton.source = "images/dss_top_button_press.png"
                    }
                    onReleased: {
                        topButton.source = "images/dss_top_button.png"
                    }
                }
            }
        }
    }
}
