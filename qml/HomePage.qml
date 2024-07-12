// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0

import org.deepin.dcc 1.0

Page {
    id: root
    SearchEdit {
        id: searchEdit
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 10
        }

        implicitWidth: (parent.width / 2) > 280 ? 280 : (parent.width / 2)
    }
    function updateMargin() {
        if (width > grid.cellWidth) {
            var count = parseInt((width - 10) / (grid.cellWidth))
            var length = dccObj.children.length
            if (length < count) {
                count = length
            }
            grid.anchors.leftMargin = (width - count * (grid.cellWidth)) / 2 + 5
        } else {
            grid.anchors.leftMargin = 10
        }
    }
    onWidthChanged: updateMargin()
    Connections {
        target: dccObj
        function onChildrenChanged() { updateMargin() }
    }
    GridView {
        id: grid

        anchors {
            top: searchEdit.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 10
            leftMargin: 10
        }

        clip: true
        cellWidth: 250
        cellHeight: 69
        activeFocusOnTab: true

        Keys.enabled: true
        Keys.onPressed: function(event) {
            switch (event.key) {
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

        highlight: Item {
            z: 2
            FocusBoxBorder {
                anchors {
                    fill: parent
                    margins: 5
                }
                radius: 8
                color: parent.palette.highlight
                visible: grid.activeFocus
            }
        }
        focus: true
        model: DccModel {
            id: dccModel
            root: dccObj
        }
        // /*
        delegate: ItemDelegate {
            // text: model.display
            width: 240
            height: 64
            anchors.margins: 10
            // radius: 8
            icon {
                name: model.item.icon
                width: 48
                height: 48
            }
            contentFlow: true
            background: DccListViewBackground {
                separatorVisible: false
            }

            content:Column {
                // anchors.left: img.right
                anchors.verticalCenter: parent.verticalCenter
                Layout.fillWidth: true
                Label {
                    id: display
                    width: parent.width
                    text: model.display
                    font: DTK.fontManager.t4
                    elide: Text.ElideRight
                }
                Label {
                    id: description
                    width: parent.width
                    visible: text !== ""
                    font: DTK.fontManager.t8
                    text: updateDescription()
                    elide: Text.ElideRight
                    function updateDescription() {
                        if (model.item.description === "" && model.item.children.length > 0) {
                            var len = model.item.children.length < 3 ? model.item.children.length : 3
                            var desc = model.item.children[0].name
                            for (var i = 1; i < len; ++i) {
                                desc += qsTr(",") + model.item.children[i].name
                            }
                            return desc + (model.item.children.length <= 3 ? "" : qsTr(" ..."))
                        }
                        return model.item.description
                    }
                    Connections {
                        target: model.item
                        function onChildrenChanged(children) { description.text = description.updateDescription() }
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.item.trigger();
                    console.log(model.item.name, model.display, model.item.icon)
                }
            }
        }
    }
}

