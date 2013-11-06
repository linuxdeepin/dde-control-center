import QtQuick 2.0

Rectangle {
    //model is a list of {"name":"somename", "url":"file:///some/url/mainfile.qml"}
    //function used to add to model A) to enforce scheme B) to allow Qt.resolveUrl in url assignments

    property string dssName: "系统设置"
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
        anchors.topMargin: 40
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
        height: 50
        color: "#252627"

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

        Image {
            id: back
            source: "images/back.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 2
            anchors.left: parent.left
            anchors.leftMargin: 20

            MouseArea {
                id: mouse
                hoverEnabled: true
                anchors.centerIn: parent
                width: 38
                height: 31
                anchors.verticalCenterOffset: -1
                onClicked: {
                    moduleItem.exampleUrl = "";
                    title.text = dssName;
                }
                Rectangle {
                    anchors.fill: parent
                    opacity: mouse.pressed ? 1 : 0
                    Behavior on opacity { NumberAnimation{ duration: 100 }}
                    gradient: Gradient {
                        GradientStop { position: 0 ; color: "#22000000" }
                        GradientStop { position: 0.2 ; color: "#11000000" }
                    }
                    border.color: "darkgray"
                    antialiasing: true
                    radius: 4
                }
            }
        }

        Text{
            id: title
            text: dssName
            anchors.left: back.left
            anchors.leftMargin: 40
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 2
            color: "#eaeaea"
            font.pixelSize: 18
            styleColor: "white"
            //style: Text.Raised
        }
    }
}
