// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
// import QtQuick.Controls 1.0
import Qt.labs.qmlmodels 1.2
// import QtQuick.Controls.Basic 2.3
// import QtLocation 5.6

import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DStyle

import Dcc 1.0

// Page
SplitView {
    id: root
    // anchors.fill: parent
         // orientation: Qt.Vertical
    // anchors.fill: parent
    orientation: Qt.Horizontal
    StyledBehindWindowBlur {
        id: leftView
        control: null
            // DccApp.mainWindow()
        // parent.parent.parent.parent.parent.parent
        // anchors.fill: parent
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        // anchors.right: title.left
        SplitView.preferredWidth: 300

        SearchEdit {
            id: searchEdit
            y: 50
            // anchors.top: title.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            // anchors.horizontalCenter:   parent.horizontalCenter
            anchors.margins: 10

            // Layout.alignment: Qt.AlignHCenter
            // implicitWidth: (parent.width / 2) > 280 ? 280 : (parent.width / 2)
        }
        ListView {
            id: list
            opacity: 0.6;
            visible: true
            anchors.top: searchEdit.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins:  10
            // anchors.centerIn: parent
            clip: true
            spacing: 5
            // anchors.left: parent.left
            // anchors.right:parent.right

            // x:bw;
            // cellWidth:250
            // cellHeight:69

            Keys.enabled: true
            Keys.onPressed: {
                switch (event.key) {
                    case Qt.Key_Escape:
                        root.close()
                        break
                    case Qt.Key_F:
                        if (root.visibility != 5)
                            root.showFullScreen()
                        else
                            root.showNormal()
                        break
                    case Qt.Key_Enter:
                    case Qt.Key_Return:
                        var obj = dccModel.getObject(currentIndex);
                        if (obj) {
                            obj.trigger();
                        }
                        break
                    default:
                        return
                }
                event.accepted = true
            }



            highlight: Rectangle {
                z: 2;
                opacity: 0.5;
                color: "lightsteelblue";
                radius: 8
            }
            focus: true
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            delegate: Rectangle{
                width: parent.width
                height: 64
                anchors.margins: 10
                radius: 8
                Image {
                     source: model.decoration.url(Qt.size(64, 64))
                 }
                DciIcon {
                    id: img
                    height: 50
                    width: 50
                    anchors.left: parent.left
                    name: model.item.icon
                    sourceSize: Qt.size(32, 32)
                }
                Text {
                    // anchors.leftMargin: 20
                    id: display
                    // width:200
                    anchors.bottom: parent.verticalCenter
                    anchors.left: img.right
                    text: model.display
                    // Layout.fillWidth: true
                }
                Text{
                    anchors.left: img.right
                    anchors.top: display.bottom
                    // width:200
                    opacity: 0.5
                    text: model.item.name
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        model.item.trigger();
                        console.log(model.item.name)
                    }
                }
            }
        }

    }
    Rectangle {
        // width: 200
        // anchors.left: leftView.right
        // anchors.right: parent.right
        // anchors.top: parent.top
        // anchors.bottom: parent.bottom
        Button {
            id: breakBut
            text: "<"
            anchors.left: parent.left
            // anchors.right: parent.right
            // anchors.top: parent.top
            anchors.verticalCenter: title.verticalCenter
            anchors.margins: 5
            height: 24
            width: 24
            // anchors.bottom: parent.bottom
            // z:2
            onClicked: dccObj.trigger()
            // DccApp.showPage("")
        }
        Label {
            id: title
            // anchors.top: parent.top
            anchors.left: breakBut.right
            anchors.leftMargin: 20
            // anchors.verticalCenter:   parent.verticalCenter
            height: 50
            verticalAlignment : Text.AlignVCenter
            text: DccApp.path
            // linkColor: linkHovered?"red":"green"
            // linkHovered:
            // font.underline : true
            onLinkActivated: DccApp.showPage(link)
            // style: "a { text-decoration: none; }"
            onLinkHovered: function (link){
                console.log(link)
            }
        }/*
        Text {
            id: title
            anchors.top: parent.top
            anchors.left: breakBut.right
            // anchors.horizontalCenter:   parent.horizontalCenter
            height: 50
            text: DccApp.path
            // linkColor: linkHovered?"red":"green"
            // linkHovered:
            font.underline : false
            onLinkActivated: DccApp.showPage(link)
            // style: "a { text-decoration: none; }"
            onLinkHovered: function (link){
                console.log(link)
            }
        }*/
        Control {
            id: rightView
            clip: true
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: title.bottom
            anchors.bottom: parent.bottom
            // anchors.fill: parent
        }
    }
    Component {
        id: menuComponent
        // property var model
        Rectangle{
            width: parent.width
            height: 64
            anchors.margins: 10
            radius: 8

            DciIcon {
                id: img
                height: 50
                width: 50
                anchors.left: parent.left
                name: model.item.icon
                sourceSize: Qt.size(32, 32)
            }
            Text {
                // anchors.leftMargin: 20
                id: display
                // width:200
                anchors.bottom: parent.verticalCenter
                anchors.left: img.right
                text: model.display
                // Layout.fillWidth: true
            }
            Text{
                anchors.left: img.right
                anchors.top: display.bottom
                // width:200
                opacity: 0.5
                text: model.item.name
            }
            Text{
                anchors.margins: 20
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                // width:200
                // opacity: 0.5
                text: ">"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.item.trigger();
                    console.log(model.item.name)
                }
            }
        }
    }
    Component {
        id: editorComponent
        Rectangle{
            width: parent.width
            height: 64
            anchors.margins: 10
            radius: 8

            DciIcon {
                id: img
                height: 50
                width: 50
                anchors.left: parent.left
                name: model.item.icon
                sourceSize: Qt.size(32, 32)
            }
            Text {
                id: display
                anchors.bottom: model.item.description !== "" ?parent.verticalCenter:anchors.bottom
                anchors.verticalCenter: model.item.description !== "" ?anchors.verticalCenter:parent.verticalCenter
                anchors.left: img.right
                text: model.display
            }
            Text{
                anchors.left: img.right
                anchors.top: display.bottom
                visible: model.item.description !== ""
                // width:200
                opacity: 0.5
                text: model.item.description
            }
            Control {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                contentItem: model.item.getSectionItem()
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.item.trigger();
                    console.log(model.item.name)
                }
            }
        }
    }
    Component {
        id: itemComponent
        Control {
            width: parent.width
            height: contentItem.height
            contentItem: model.item.getSectionItem()
        }
    }
    Component {
        id: rightLayout
        ListView {
            anchors.fill: parent
            // highlight: Rectangle {
            //     z: 2;
            //     opacity: 0.5;
            //     color: "lightsteelblue";
            //     radius: 8
            // }
            focus: true
            model: DccModel {
                id: dccModel
                root: dccObj
            }
            //禁止listview控件回弹，解决白色空白出现
            boundsBehavior: Flickable.StopAtBounds
            delegate: DelegateChooser {
                role: "pageType"
                DelegateChoice { roleValue: DccObject.Menu; delegate: menuComponent }
                DelegateChoice { roleValue: DccObject.Editor; delegate: editorComponent }
                DelegateChoice { roleValue: DccObject.Item; delegate: itemComponent }
            }
        }
    }
    function updateRightView() {
        var activeObj = DccApp.activeObject
        if (activeObj == dccObj)
            return;

        if (activeObj.page == null) {
            activeObj.page = rightLayout
        }
        rightView.contentItem = activeObj.getSectionItem()
    }
    Connections {
        target: DccApp
        onActiveObjectChanged: updateRightView()
    }
    Component.onCompleted: updateRightView()

}

