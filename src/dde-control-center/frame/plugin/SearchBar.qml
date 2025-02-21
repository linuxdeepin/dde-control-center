// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import org.deepin.dtk 1.0
import QtQuick.Layouts 1.15

import org.deepin.dtk.style 1.0 as DS

Rectangle {
    id: root
    property alias model: view.model
    property alias edit: searchEdit
    signal clicked(var model)

    implicitHeight: 32
    implicitWidth: (parent.width / 2) > 240 ? 240 : (parent.width / 2)
    color: "transparent"
    radius: DS.Style.control.radius
    border.color: palette.light // "#E1E7EB" #D2E4F2
    border.width: 1
    opacity: 0.7

    onVisibleChanged: {
        searchEdit.text = ""
        popup.close()
    }

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
        Keys.enabled: true
        Keys.onPressed: function (event) {
            switch (event.key) {
            case Qt.Key_Escape:
                popup.close()
                break
            case Qt.Key_Down:
            {
                let cIndex = view.currentIndex + 1
                if (cIndex < 0) {
                    cIndex = 0
                }
                view.currentIndex = cIndex
            }
            break
            case Qt.Key_Up:
            {
                let cIndex = view.currentIndex - 1
                if (cIndex < 0) {
                    cIndex = 0
                }
                view.currentIndex = cIndex
            }
            break
            case Qt.Key_Return:
            case Qt.Key_Enter:
                if (view.currentItem) {
                    view.currentItem.clicked()
                }
                break
            default:
                return
            }
            event.accepted = true
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
                checked: ListView.isCurrentItem
                contentFlow: true
                content: DccLabel {
                    text: model.display ? model.display : ""
                }
                onClicked: {
                    root.clicked(model)
                    popup.close()
                }
                background: DccItemBackground {
                    separatorVisible: model.isBegin !== undefined ? model.isBegin : false
                }
            }
        }
        enter: Transition {
            NumberAnimation {
                property: "opacity"
                from: 0.0
                to: 1.0
                duration: 150
            }
            NumberAnimation {
                property: "scale"
                from: 0.0
                to: 1.0
                duration: 150
            }
        }

        exit: Transition {
            NumberAnimation {
                property: "opacity"
                to: 0.0
                duration: 150
            }
            NumberAnimation {
                property: "scale"
                to: 0.0
                duration: 150
            }
        }
    }
}
