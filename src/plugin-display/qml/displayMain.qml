// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0
import org.deepin.dcc.defApp 1.0

DccObject {
    id: root
    property var screen: dccData.virtualScreens[0]

    ListModel {
        id: resolutionModel
    }
    ListModel {
        id: rateModel
    }
    ListModel {
        id: fillModel
    }
    ListModel {
        id: modeModel
        ListElement {
            text: qsTr("Duplicate")
            value: "MERGE" // 1
        }
        ListElement {
            text: qsTr("Extend")
            value: "EXTEND" // 2
        }
    }
    function updateResolutionModel() {
        resolutionModel.clear()
        for (let resolution of screen.resolutionList) {
            if (resolution.width === screen.bestResolution.width && resolution.height === screen.bestResolution.height) {
                resolutionModel.insert(0, {
                                           "text": resolution.width + "×" + resolution.height + qsTr(" (Recommended)"),
                                           "value": resolution
                                       })
            } else {
                resolutionModel.append({
                                           "text": resolution.width + "×" + resolution.height,
                                           "value": resolution
                                       })
            }
        }
    }
    function updateRateModel() {
        rateModel.clear()
        for (let rate of screen.rateList) {
            if (rate === screen.bestRate) {
                rateModel.append({
                                     "text": (Math.round(rate * 100) / 100) + qsTr("Hz") + qsTr(" (Recommended)"),
                                     "value": rate
                                 })
            } else {
                rateModel.append({
                                     "text": (Math.round(rate * 100) / 100) + qsTr("Hz"),
                                     "value": rate
                                 })
            }
        }
    }
    function updateFillModel() {
        fillModel.clear()
        for (let fillmode of screen.availableFillModes) {
            switch (fillmode) {
            case "None":
                fillModel.append({
                                     "text": qsTr("Default"),
                                     "icon": "Default",
                                     "value": fillmode
                                 })
                break
            case "Full aspect":
                fillModel.append({
                                     "text": qsTr("Fit"),
                                     "icon": "Fit",
                                     "value": fillmode
                                 })
                break
            case "Full":
                fillModel.append({
                                     "text": qsTr("Stretch"),
                                     "icon": "Stretch",
                                     "value": fillmode
                                 })
                break
            case "Center":
                fillModel.append({
                                     "text": qsTr("Center"),
                                     "icon": "Center",
                                     "value": fillmode
                                 })
                break
            }
        }
    }
    function updateModeModel() {
        if (modeModel.count > 2) {
            modeModel.remove(2, modeModel.count - 2)
        }
        for (let screen of dccData.screens) {
            modeModel.append({
                                 "text": qsTr("Only on %1").arg(screen.name),
                                 "value": screen.name
                             })
        }
    }
    Connections {
        target: screen
        function onResolutionListChanged() {
            updateResolutionModel()
        }
        function onRateListChanged() {
            updateRateModel()
        }
        function onAvailableFillModesChanged() {
            updateFillModel()
        }
    }
    Connections {
        target: dccData
        function onScreensChanged() {
            updateModeModel()
        }
    }
    onScreenChanged: {
        updateResolutionModel()
        updateRateModel()
        updateFillModel()
    }
    Component.onCompleted: {
        updateResolutionModel()
        updateRateModel()
        updateFillModel()
        updateModeModel()
    }

    DccTitleObject {
        name: "multipleDisplays"
        parentName: "display"
        displayName: qsTr("Multiple Displays Settings")
        weight: 10
        visible: dccData.screens.length > 1
    }
    DccObject {
        name: "monitorControl"
        parentName: "display"
        weight: 20
        visible: false && dccData.screens.length > 1
        pageType: DccObject.Item
        page: Rectangle {
            implicitHeight: 240
            // tr("Recognize") // 识别屏幕
            Rectangle {
                x: 30
                implicitHeight: 40
                implicitWidth: 30
                color: "red"
            }
            Rectangle {
                x: 60
                implicitHeight: 40
                implicitWidth: 30
                color: "green"
            }
        }
    }
    DccObject {
        name: "displayMultipleDisplays"
        parentName: "display"
        weight: 30
        visible: dccData.screens.length > 1
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "mode"
            parentName: "display/displayMultipleDisplays"
            displayName: qsTr("Mode")
            weight: 10
            visible: dccData.screens.length > 1 && dccData.isX11
            pageType: DccObject.Editor
            page: ComboBox {
                textRole: "text"
                valueRole: "value"
                model: modeModel
                function indexOfMode(mode) {
                    for (var i = 0; i < model.count; i++) {
                        if (model.get(i).value === mode) {
                            return i
                        }
                    }
                    return -1
                }
                currentIndex: indexOfMode(dccData.displayMode)
                onActivated: {
                    dccData.displayMode = currentValue
                }
            }
        }
        DccObject {
            name: "mainScreen"
            parentName: "display/displayMultipleDisplays"
            displayName: qsTr("Main Screen")
            weight: 20
            pageType: DccObject.Editor
            visible: dccData.virtualScreens.length > 1
            page: ComboBox {
                textRole: "name"
                model: dccData.virtualScreens
                function indexOfScreen(primary) {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i].name === primary.name) {
                            return i
                        }
                    }
                    return -1
                }

                currentIndex: dccData.primaryScreen ? indexOfScreen(dccData.primaryScreen) : -1
                onActivated: {
                    dccData.primaryScreen = currentValue
                }
            }
        }
    }
    DccTitleObject {
        name: "screenTitle"
        parentName: "display"
        displayName: qsTr("Display And Layout")
        weight: 40
    }
    DccObject {
        name: "screenTab"
        parentName: "display"
        weight: 50
        visible: dccData.virtualScreens.length > 1
        pageType: DccObject.Item
        Component {
            id: indicator
            ScreenIndicator {}
        }
        Component {
            id: itmeoutDialog
            TimeoutDialog {}
        }

        page: ScreenTab {
            model: dccData.virtualScreens
            screen: root.screen
            onScreenChanged: {
                if (this.screen && this.screen !== root.screen) {
                    root.screen = this.screen
                    if (dccData.isX11) {
                        indicator.createObject(this, {
                                                   "screen": getQtScreen(root.screen)
                                               }).show()
                    }
                }
            }
            function getQtScreen(screen) {
                for (var s of Qt.application.screens) {
                    if (s.virtualX === screen.x && s.virtualY === screen.y && s.width === screen.currentResolution.width && s.height === screen.currentResolution.height) {
                        return s
                    }
                }
                return null
            }
        }
    }
    DccObject {
        name: "brightnessGroup"
        parentName: "display"
        displayName: qsTr("Brightness")
        weight: 60
        visible: false
        pageType: DccObject.Item
        page: Rectangle {
            implicitHeight: 30
        }
    }
    DccObject {
        name: "screenGroup"
        parentName: "display"
        weight: 70
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "brightness"
            parentName: "display/screenGroup"
            displayName: qsTr("Brightness")
            weight: 10
            visible: false
            pageType: DccObject.Editor
            page: Slider {}
        }

        DccObject {
            name: "displayResolution"
            parentName: "display/screenGroup"
            displayName: qsTr("Resolution") // 分辨率
            weight: 20
            pageType: DccObject.Editor
            page: ComboBox {
                model: resolutionModel
                textRole: "text"
                valueRole: "value"
                function indexOfSize(currentSize) {
                    for (var i = 0; i < model.count; i++) {
                        let v = model.get(i)
                        if (v.value.width === currentSize.width && v.value.height === currentSize.height) {
                            return i
                        }
                    }
                    return -1
                }

                currentIndex: indexOfSize(screen.currentResolution)
                onActivated: {
                    screen.currentResolution = currentValue
                    if (dccData.isX11) {
                        itmeoutDialog.createObject(this, {
                                                       "screenX": root.screen.x,
                                                       "screenY": root.screen.y,
                                                       "screenWidth": root.screen.currentResolution.width,
                                                       "screenHeight": root.screen.currentResolution.height
                                                   }).show()
                    }
                }
            }
        }
        DccObject {
            name: "resizeDesktop"
            parentName: "display/screenGroup"
            displayName: qsTr("Resize Desktop") // 屏幕显示
            weight: 30
            visible: false
            pageType: DccObject.Editor
            page: D.ComboBox {
                id: control
                textRole: "text"
                valueRole: "value"
                // iconNameRole: "icon"
                model: fillModel
                function indexOfFill(currentFillMode) {
                    for (var i = 0; i < model.count; i++) {
                        let v = model.get(i)
                        if (v.value === currentFillMode) {
                            return i
                        }
                    }
                    return -1
                }
                currentIndex: indexOfFill(screen.currentFillMode)
                onActivated: {
                    screen.currentFillMode = currentValue
                    if (dccData.isX11) {
                        itmeoutDialog.createObject(this, {
                                                       "screenX": root.screen.x,
                                                       "screenY": root.screen.y,
                                                       "screenWidth": root.screen.currentResolution.width,
                                                       "screenHeight": root.screen.currentResolution.height
                                                   }).show()
                    }
                }
            }
        }
        DccObject {
            name: "displayRefreshRate"
            parentName: "display/screenGroup"
            displayName: qsTr("Refresh Rate") // 刷新率
            weight: 40
            pageType: DccObject.Editor
            page: ComboBox {
                textRole: "text"
                valueRole: "value"
                model: rateModel
                function indexOfRate(currentRate) {
                    for (var i = 0; i < model.count; i++) {
                        let v = model.get(i)
                        if (v.value === currentRate) {
                            return i
                        }
                    }
                    return -1
                }
                currentIndex: indexOfRate(screen.currentRate)
                onActivated: {
                    screen.currentRate = currentValue
                    if (dccData.isX11) {
                        itmeoutDialog.createObject(this, {
                                                       "screenX": root.screen.x,
                                                       "screenY": root.screen.y,
                                                       "screenWidth": root.screen.currentResolution.width,
                                                       "screenHeight": root.screen.currentResolution.height
                                                   }).show()
                    }
                }
            }
        }
        DccObject {
            name: "displayRotate"
            parentName: "display/screenGroup"
            displayName: qsTr("Rotation") // 方向
            weight: 50
            visible: dccData.isX11
            pageType: DccObject.Editor
            page: ComboBox {
                textRole: "text"
                valueRole: "value"
                model: [{
                        "text": qsTr("Standard"),
                        "value": 1
                    }, {
                        "text": qsTr("90°"),
                        "value": 2
                    }, {
                        "text": qsTr("180°"),
                        "value": 4
                    }, {
                        "text": qsTr("270°"),
                        "value": 8
                    }]
                function indexOfRotate(rotate) {
                    for (var i = 0; i < model.length; i++) {
                        let v = model[i]
                        if (v.value === rotate) {
                            return i
                        }
                    }
                    return -1
                }

                currentIndex: indexOfRotate(screen.rotate)
                onActivated: {
                    screen.rotate = currentValue
                }
            }
        }
        DccObject {
            name: "displayScaling"
            parentName: "display/screenGroup"
            displayName: qsTr("Display Scaling") //"缩放"
            weight: 60
            visible: false
            pageType: DccObject.Editor
            page: ComboBox {}
            // qsTr("The monitor only supports 100% display scaling")
        }
    }
    DccTitleObject {
        name: "displayColorTemperature"
        parentName: "display"
        displayName: qsTr("Eye Comfort")
        weight: 80
        visible: false
    }
    DccObject {
        name: "eyeComfort"
        parentName: "display"
        displayName: qsTr("Eye Comfort")
        description: qsTr("Adjust screen display to warmer colors, reducing screen blue light")
        weight: 90
        visible: false
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: Switch {}
    }
    DccObject {
        name: "eyeComfortGroup"
        parentName: "display"
        weight: 100
        visible: false
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "time"
            parentName: "display/eyeComfortGroup"
            displayName: qsTr("Time")
            weight: 10
            pageType: DccObject.Editor
            page: ComboBox {
                model: [qsTr("All day"), qsTr("Sunset to Sunrise"), qsTr("Custom Time")]
            }
        }
        DccObject {
            name: "timeFrame"
            parentName: "display/eyeComfortGroup"
            // displayName: qsTr("自定义时间")
            weight: 20
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    text: qsTr("from")
                }
                DccTimeRange {// id: hourTime
                    // hour: sysItemModel.timeStart.split(":")[0]
                    // minute: sysItemModel.timeStart.split(":")[1]
                    // onTimeChanged: sysItemModel.timeStart = timeString
                }
                Label {
                    text: qsTr("to")
                }
                DccTimeRange {// id: hourTime
                    // hour: sysItemModel.timeStart.split(":")[0]
                    // minute: sysItemModel.timeStart.split(":")[1]
                    // onTimeChanged: sysItemModel.timeStart = timeString
                }
            }
        }
        DccObject {
            name: "colorTemperature"
            parentName: "display/eyeComfortGroup"
            displayName: qsTr("Color Temperature")
            weight: 30
            pageType: DccObject.Editor
            page: Slider {
                Layout.alignment: Qt.AlignVCenter
                implicitHeight: 24
            }
        }
    }
}
