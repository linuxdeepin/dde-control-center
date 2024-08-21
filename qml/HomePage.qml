// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0
import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

Page {
    id: root
    property bool contentVisible: true

    property real cellWidth: 240
    property real cellHeight: 64
    property real cellSpacing: 10

    header: Item {
        implicitHeight:50
        Rectangle {
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                margins: 10
            }

            visible: contentVisible
            implicitHeight: 32
            implicitWidth: (parent.width / 2) > 240 ? 240 : (parent.width / 2)
            color: "transparent"
            radius: DS.Style.control.radius
            border.color: palette.light // "#E1E7EB"
            border.width: 1

            SearchEdit {
                id: searchEdit
                anchors.fill: parent
                anchors.margins: 1
                activeFocusOnTab: true

                // focus: true
                placeholderTextColor: palette.brightText
                padding: 1

                property Palette nomalPalette: Palette {
                    normal: ("#FCFCFC")
                    normalDark: ("#0C0C0C")
                    hovered: (palette.text)
                    hoveredDark: ("#FCFCFC")
                }

                backgroundColor: nomalPalette
            }
        }
        Rectangle {
            id: separator
            visible: contentVisible
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: palette.shadow // "#F2F2F2"
            // color: palette.placeholderText
            height: 2
        }
    }

    function updateMargin() {
        if (width > grid.cellWidth) {
            var count = parseInt((width - root.cellSpacing) / (grid.cellWidth))
            var length = dccObj.children.length
            if (length < count) {
                count = length
            }
            grid.anchors.leftMargin = (width - count * (grid.cellWidth)) / 2 + (root.cellSpacing / 2)
        } else {
            grid.anchors.leftMargin = root.cellSpacing
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
            fill: parent
            topMargin: root.cellSpacing
            leftMargin: root.cellSpacing
        }

        clip: true
        cellWidth: root.cellWidth + root.cellSpacing
        cellHeight: root.cellHeight + root.cellSpacing

        visible: contentVisible

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
            width: root.cellWidth
            height: root.cellHeight
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
                    Layout.maximumWidth: 160
                    Label {
                        id: display
                        Layout.maximumWidth: 160
                        text: model.display
                        color: palette.brightText
                        elide: Text.ElideRight
                    }
                    Label {
                        id: description
                        Layout.maximumWidth: 160
                        visible: text !== ""
                        font: DTK.fontManager.t10
                        text: updateDescription()
                        color: palette.brightText
                        opacity: 0.5
                        elide: Text.ElideRight
                        function updateDescription() {
                            if (model.item.description === "" && model.item.children.length > 0) {
                                var len = model.item.children.length < 3 ? model.item.children.length : 3
                                var desc = model.item.children[0].displayName
                                for (var i = 1; i < len; ++i) {
                                    desc += qsTr(",") + model.item.children[i].displayName
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
