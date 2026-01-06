// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import org.deepin.dtk 1.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

Rectangle {
    id: control
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
        function setViewIndex(viewIndex) {
            if (viewIndex < 0) {
                viewIndex = view.count - 1
                view.positionViewAtEnd()
            } else if (viewIndex >= view.count) {
                viewIndex = 0
                view.positionViewAtBeginning()
            }
            view.currentIndex = viewIndex
        }

        anchors.fill: parent
        anchors.margins: 1
        activeFocusOnTab: true

        // focus: true
        placeholderTextColor: palette.brightText
        padding: 1

        DropArea {
            anchors.fill: parent
            onDropped: function(drop) {
                if (drop.hasText) {
                    searchEdit.text = drop.text
                    drop.acceptProposedAction()
                }
            }
        }

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
                setViewIndex(view.currentIndex + 1)
                break
            case Qt.Key_Up:
                setViewIndex(view.currentIndex - 1)
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
        height: (view.count > 7 ? 7 : view.count) * 32 + 15
        padding: 5
        ListView {
            id: view
            clip: true
            anchors.fill: parent
            spacing: 0
            delegate: D.ItemDelegate {
                implicitWidth: parent ? parent.width : 0
                implicitHeight: 32
                topInset: 0
                bottomInset: 0
                topPadding: 0
                bottomPadding: 0
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
                    control.clicked(model)
                    popup.close()
                }
                background: DccItemBackground {
                    separatorVisible: model.isEnd !== undefined ? model.isEnd : false
                    bgMargins: 0
                    backgroundType: DccObject.Hover
                }
            }
            ScrollBar.vertical: ScrollBar {}
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
