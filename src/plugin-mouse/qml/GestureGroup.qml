// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    property bool backgroundVisible: true
    property bool showPlayBtn: true
    signal clicked(int index, bool checked)
    signal comboIndexChanged(int index, var actionDec)
    signal hoveredChanged(int index, var actionDec, bool hovered)

    color: "transparent"
    implicitHeight: layoutView.height
    Layout.fillWidth: true
    ColumnLayout {
        id: layoutView
        width: parent.width
        clip: true
        spacing: 0
        Repeater {
            id: repeater
            delegate: D.ItemDelegate {
                id: itemCtl
                Layout.fillWidth: true
                leftPadding: 10
                rightPadding: 10
                topPadding: 0
                bottomPadding: 0
                implicitHeight: 36
                topInset: index === 0 ? 2 : 0
                cascadeSelected: true
                backgroundVisible: root.backgroundVisible
                text: model.descriptionRole
                icon.name: model.iconRole
                hoverEnabled: true
                
                // 使用框架标准函数动态设置corners
                corners: getCornersForBackground(index, repeater.count)

                property var comboMoel: model.actionListRole
                property int comboIndex: model.actionsIndexRole
                property var comboItem: null
                property int delegateIndex: index  // Save delegate's index to avoid being overridden by ComboBox's index parameter
                content: D.ComboBox {
                    id: combo
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    model: comboMoel
                    textRole: "actionText"
                    valueRole: "actionValue"
                    currentIndex: comboIndex
                    editable: false
                    flat: true
                    onActivated: {
                        root.comboIndexChanged(itemCtl.delegateIndex, combo.currentValue)
                    }
                    Component.onCompleted: comboItem = this
                }
                
                background: DccItemBackground {
                    backgroundType: DccObject.Hover
                    separatorVisible: index < (repeater.count - 1)  // 动态条件：最后一行不显示分割线
                }

                onHoveredChanged: {
                    root.hoveredChanged(delegateIndex, comboItem.currentValue, hovered)
                }

                onClicked: {
                    root.clicked(delegateIndex, !model.isChecked)
                }
            }
        }
    }
}
