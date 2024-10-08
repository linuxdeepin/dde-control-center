// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        name: "fontSize"
        parentName: "personalization/font"
        displayName: qsTr("Size")
        hasBackground: true
        weight: 10
        pageType: DccObject.Item
        page: ColumnLayout {
            Layout.fillHeight: true
            Label {
                Layout.topMargin: 10
                font: D.DTK.fontManager.t7
                text: dccObj.displayName
                Layout.leftMargin: 10
            }
            D.TipsSlider {
                id: fontSizeSlider
                readonly property var fontSizeModel: dccData.model.compactDisplay ? [10, 11, 12, 13, 14, 15, 16] : [11, 12, 13, 14, 15, 16, 18, 20]
                Layout.preferredHeight: 90
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 10
                Layout.fillWidth: true
                tickDirection: D.TipsSlider.TickDirection.Back
                slider.handleType: Slider.HandleType.ArrowBottom
                slider.value: fontSizeModel.indexOf(dccData.model.fontSizeModel.size)
                slider.from: 0
                slider.to: fontSizeModel.length - 1
                slider.live: true
                slider.stepSize: 1
                slider.snapMode: Slider.SnapAlways
                ticks: Repeater {
                    model: fontSizeSlider.fontSizeModel.length
                    D.SliderTipItem {
                        text: fontSizeSlider.fontSizeModel[index]
                        highlight: fontSizeSlider.slider.value === index
                    }

                }
                slider.onValueChanged: {
                    dccData.worker.setFontSize(fontSizeSlider.fontSizeModel[fontSizeSlider.slider.value])
                }
            }
        }
    }

    DccObject {
        name: "standardFont"
        parentName: "personalization/font"
        displayName: qsTr("Standard Font")
        weight: 100
        hasBackground: true
        pageType: DccObject.Editor
        page: FontCombobox {
            flat: true
            model: dccData.model.standardFontModel.fontList
            textRole: "Name"
            valueRole: "Id"
            currentIndex: indexOfValue(dccData.model.standardFontModel.fontName)
            onCurrentIndexChanged: {
                dccData.worker.setDefault(model[currentIndex]);
            }

            function indexOfValue(value) {
                for (var i = 0; i < model.length; i++) {
                    if (model[i][valueRole] === value) {
                        return i
                    }
                }
                return -1
            }
        }
    }

    DccObject {
        name: "monoFont"
        parentName: "personalization/font"
        displayName: qsTr("Monospaced Font")
        weight: 200
        hasBackground: true
        pageType: DccObject.Editor
        page: FontCombobox {
            flat: true
            model: dccData.model.monoFontModel.fontList
            textRole: "Name"
            valueRole: "Id"
            currentIndex: indexOfValue(dccData.model.monoFontModel.fontName)
            onCurrentIndexChanged: {
                dccData.worker.setDefault(model[currentIndex]);
            }

            function indexOfValue(value) {
                for (var i = 0; i < model.length; i++) {
                    if (model[i][valueRole] === value) {
                        return i
                    }
                }
                return -1
            }
        }
    }
}
