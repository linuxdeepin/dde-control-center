// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.11
import QtQuick.Layouts 1.15
import QtQuick.Templates as T
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

T.Control {
    id: control
    property string text: "ImageCheckBox"
    property bool checked: true
    property string imageName: ""
    implicitWidth: contentItem.implicitWidth
    implicitHeight: contentItem.implicitHeight
    MouseArea {
        anchors.fill: parent
        onClicked: {
            checked = !checked
        }
    }

    ColumnLayout {
        id: contentItem
        Image {
            Layout.alignment: Qt.AlignCenter
            sourceSize: Qt.size(96, 65)
            mipmap: true
            source: D.DTK.themeType === D.ApplicationHelper.LightType ?
                "qrc:/icons/deepin/builtin/light/icons/" + imageName +"_84px.png" : "qrc:/icons/deepin/builtin/dark/icons/" + imageName +"_84px.png"
        }
        RowLayout {
            Layout.alignment: Qt.AlignCenter
            D.DciIcon {
                Layout.alignment: Qt.AlignCenter
                palette: control.D.DTK.makeIconPalette(control.palette)
                mode: control.D.ColorSelector.controlState
                theme: control.D.ColorSelector.controlTheme
                name: control.checked ? "item_checked" :  "item_unchecked"
                fallbackToQIcon: false
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                text: control.text
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                color: control.palette.windowText
            }
        }
    }
}
