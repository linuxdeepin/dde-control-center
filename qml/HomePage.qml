// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
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
        activeFocusOnTab: true
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
        function onChildrenChanged() {
            updateMargin()
        }
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
        cellWidth: 225
        cellHeight: 68

        activeFocusOnTab: true

        Keys.enabled: true
        Keys.onPressed: function (event) {
            switch (event.key) {
            case Qt.Key_Enter:
            case Qt.Key_Return:
                var obj = dccModel.getObject(currentIndex)
                if (obj) {
                    obj.trigger()
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
        delegate: ItemDelegate {
            width: 215
            height: 58
            padding: 12
            icon {
                name: model.item.icon
                width: 32
                height: 32
            }
            contentFlow: true
            background: DccListViewBackground {
                separatorVisible: false
            }
            clip: true

            content: RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    Layout.leftMargin: 5
                    Layout.maximumWidth: 130
                    Label {
                        id: display
                        Layout.maximumWidth: 130
                        text: model.display
                        color: palette.brightText
                        elide: Text.ElideRight
                    }
                    Label {
                        id: description
                        Layout.maximumWidth: 130
                        visible: text !== ""
                        font: DTK.fontManager.t10
                        text: updateDescription()
                        color: palette.brightText
                        opacity: 0.5
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
                            function onChildrenChanged(children) {
                                description.text = description.updateDescription()
                            }
                        }
                    }
                }
                Rectangle {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: -2
                    visible: model.item.badge !== 0
                    height: 16
                    width: 16
                    radius: 8
                    color: "red"
                }
            }
            onClicked: {
                model.item.trigger()
                console.log(model.item.name, model.display, model.item.icon)
            }
        }
    }
}
