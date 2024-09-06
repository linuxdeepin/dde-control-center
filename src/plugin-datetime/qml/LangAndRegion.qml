// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import org.deepin.dcc 1.0
import org.deepin.dtk 1.0 as D

// 语言和地区
DccObject {
    DccObject {
        id: langRegion
        name: "langAndRegionChooser"
        parentName: "langAndRegion"
        weight: 10
        pageType: DccObject.Item
        page: DccGroupView {}

        DccObject {
            name: "langAndRegionCombo"
            parentName: "langAndRegionChooser"
            displayName: qsTr("language and region")
            weight: 10
            hasBackground: true
            icon: "dcc_nav_datetime"
            pageType: DccObject.Editor
            // TODO: Combobox with searchedit
            page: ComboBox {
                id: langRegionCombo
                // implicitWidth: textMetrics.advanceWidth + 30
                flat: true
                model: dccData.languagesAndRegions()
                currentIndex: 0

                onCurrentIndexChanged: {
                    console.log("inputDevice  Selected index:", currentIndex)
                }
            }
        }
    }

    DccObject {
        name: "langlistTiltle"
        parentName:  "langAndRegion"
        displayName: qsTr("Language list")
        weight: 20
        hasBackground: false
        pageType: DccObject.Item

        page: Label {
            text: dccObj.displayName
            font: D.DTK.fontManager.t4
        }

        DccObject {
            id: langlistItems
            name: "langItems"
            parentName: "langAndRegion"
            weight: 30
            pageType: DccObject.Item
            page: DccGroupView {}

            DccObject {
                name: "langItem0"
                parentName: "langItems"
                displayName: qsTr("Chinese-Chinese")
                weight: 10
                hasBackground: true
                pageType: DccObject.Editor
                page: Button {
                    id: menuBtn
                    implicitWidth: 30
                    // TODO: use icon
                    text: "..."
                }
            }
        }
    }
}
