// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

Item {
    id: item
    property var comboModel: [""]
    property int comboCurrentIndex: -1
    property bool isDecimalSymbol: false
    property bool isDigitGroupingSymbol: false
    // property string textRole
    implicitHeight: 30
    implicitWidth: 280
    signal comboBoxActivated(int index)

    // 获取过滤后的模型数据
    function getFilteredModel() {
        if (!dccData) {
            return item.comboModel
        }
        
        if (item.isDecimalSymbol) {
            return dccData.getFilteredDecimalSymbols()
        } else if (item.isDigitGroupingSymbol) {
            return dccData.getFilteredSeparatorSymbols()
        }
        
        return item.comboModel
    }

    // 将过滤后模型的索引转换为原始模型的索引
    function getOriginalIndex(filteredIndex) {
        if (!dccData || (!item.isDecimalSymbol && !item.isDigitGroupingSymbol)) {
            return filteredIndex
        }
        
        var filteredModel = getFilteredModel()
        var originalModel = item.comboModel
        
        if (filteredIndex < 0 || filteredIndex >= filteredModel.length) {
            return -1
        }
        
        var selectedValue = filteredModel[filteredIndex]
        return originalModel.indexOf(selectedValue)
    }

    RowLayout {
        anchors.fill: parent
        
        Item {
            Layout.fillWidth: true
        }
        
        ComboBox {
            id: comboBox
            visible: item.comboModel.length > 1
            flat: true
            implicitWidth: 220
            model: getFilteredModel()
            currentIndex: {
                if (!dccData || (!item.isDecimalSymbol && !item.isDigitGroupingSymbol)) {
                    return comboCurrentIndex
                }
                
                // 对于过滤模型，需要将原始索引转换为过滤后的索引
                if (comboCurrentIndex >= 0 && comboCurrentIndex < item.comboModel.length) {
                    var currentValue = item.comboModel[comboCurrentIndex]
                    var filteredModel = getFilteredModel()
                    return filteredModel.indexOf(currentValue)
                }
                return -1
            }
            hoverEnabled: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            onActivated: function (index) {
                // 如果使用了过滤模型，需要转换索引
                var originalIndex = getOriginalIndex(index)
                item.comboBoxActivated(originalIndex)
            }
            
            // 监听符号变化，实时更新模型
            Connections {
                target: dccData
                function onSymbolChanged(format, symbol) {
                    // 当小数点或分隔符符号变化时，更新下拉列表
                    if ((format === 9 && item.isDigitGroupingSymbol) || // DecimalSymbol changed, update separator list
                        (format === 10 && item.isDecimalSymbol)) {      // DigitGroupingSymbol changed, update decimal list
                        var newModel = getFilteredModel()
                        comboBox.model = newModel
                    }
                }
            }
        }

        Label {
            id: label
            visible: item.comboModel.length === 1
            text: item.comboModel[0]
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.rightMargin: 10
        }
    }
}
