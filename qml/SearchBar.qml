// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import org.deepin.dtk 1.0
import QtQuick.Layouts 1.15

import org.deepin.dtk.style 1.0 as DS

import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: view.model
    signal clicked(var model)

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
        onTextChanged: {
            if (text === "") {
                popup.close()
            } else {
                model.setFilterRegularExpression(text)
                if (view.count > 0) {
                    popup.open()
                } else {
                    popup.close()
                }
            }
        }
    }
    Popup {
        id: popup
        y: 35
        width: searchEdit.width > 308 ? searchEdit.width : 308
        padding: 10
        ListView {
            id: view
            clip: true
            anchors.fill: parent
            delegate: ItemDelegate {
                implicitWidth: parent ? parent.width : 0
                implicitHeight: 32
                backgroundVisible: true
                corners: getCornersForBackground(index, view.count)
                icon.name: model.decoration
                // text: model.display
                contentFlow: true
                content: DccLabel {
                    text: model.display
                }
                onClicked: {
                    root.clicked(model)
                }
                background: DccListViewBackground {
                    separatorVisible: model.isBegin
                }
            }
        }
    }
}
