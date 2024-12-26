// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

DccObject {
    id: root
    property var screen: dccData.virtualScreens[0]
    property var scaleModelConst: [{
            "text": qsTr("100%"),
            "value": 1.0
        }, {
            "text": qsTr("125%"),
            "value": 1.25
        }, {
            "text": qsTr("150%"),
            "value": 1.50
        }, {
            "text": qsTr("175%"),
            "value": 1.75
        }, {
            "text": qsTr("200%"),
            "value": 2.0
        }, {
            "text": qsTr("225%"),
            "value": 2.25
        }, {
            "text": qsTr("250%"),
            "value": 2.50
        }, {
            "text": qsTr("275%"),
            "value": 2.75
        }, {
            "text": qsTr("300%"),
            "value": 3.0
        }]

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
    function getFillModel(availableFillModes) {
        var fillModel = []
        for (let fillmode of availableFillModes) {
            switch (fillmode) {
            case "None":
                fillModel.push({
                                   "text": qsTr("Default"),
                                   "icon": "Default",
                                   "value": fillmode
                               })
                break
            case "Full aspect":
                fillModel.push({
                                   "text": qsTr("Fit"),
                                   "icon": "Fit",
                                   "value": fillmode
                               })
                break
            case "Full":
                fillModel.push({
                                   "text": qsTr("Stretch"),
                                   "icon": "Stretch",
                                   "value": fillmode
                               })
                break
            case "Center":
                fillModel.push({
                                   "text": qsTr("Center"),
                                   "icon": "Center",
                                   "value": fillmode
                               })
                break
            }
        }
        return fillModel
    }
    function getModeModel(screens) {
        if (modeModel.count > 2) {
            modeModel.remove(2, modeModel.count - 2)
        }
        for (let screen of screens) {
            modeModel.append({
                                 "text": qsTr("Only on %1").arg(screen.name),
                                 "value": screen.name
                             })
        }
        return modeModel
    }
    function getResolutionModel(resolutionList, bestResolution) {
        var resolutionModel = []
        for (let resolution of resolutionList) {
            if (resolution.width === bestResolution.width && resolution.height === bestResolution.height) {
                resolutionModel.unshift({
                                            "text": resolution.width + "×" + resolution.height + qsTr(" (Recommended)"),
                                            "value": resolution
                                        })
            } else {
                resolutionModel.push({
                                         "text": resolution.width + "×" + resolution.height,
                                         "value": resolution
                                     })
            }
        }
        return resolutionModel
    }
    function getRateModel(rateList, bestRate) {
        var rateModel = []
        for (let rate of rateList) {
            if (rate === bestRate) {
                rateModel.push({
                                   "text": (Math.round(rate * 100) / 100) + qsTr("Hz") + qsTr(" (Recommended)"),
                                   "value": rate
                               })
            } else {
                rateModel.push({
                                   "text": (Math.round(rate * 100) / 100) + qsTr("Hz"),
                                   "value": rate
                               })
            }
        }
        return rateModel
    }
    function getScaleModel(maxScale, scale) {
        var scaleModel = []
        for (let scaleItem of scaleModelConst) {
            if (scaleItem.value <= maxScale) {
                scaleModel.push(scaleItem)
            }
        }
        return scaleModel
    }
    function indexOfScale(model, scale) {
        for (var i = 0; i < model.length; i++) {
            let v = model[i]
            if (v.value === scale) {
                return i
            }
        }
        return model.length - 1
    }
    Connections {
        target: dccData
        function onVirtualScreensChanged() {
            if (!dccData.virtualScreens.includes(screen)) {
                screen = dccData.virtualScreens[0]
            }
        }
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
                flat: true
                textRole: "text"
                valueRole: "value"
                model: getModeModel(dccData.screens)
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
                flat: true
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
        visible: dccData.screens.length > 1
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
                flat: true
                model: root.getResolutionModel(screen.resolutionList, screen.bestResolution)
                textRole: "text"
                valueRole: "value"
                function indexOfSize(model, currentSize) {
                    for (var i = 0; i < model.length; i++) {
                        let v = model[i]
                        if (v.value.width === currentSize.width && v.value.height === currentSize.height) {
                            return i
                        }
                    }
                    return -1
                }

                currentIndex: indexOfSize(model, screen.currentResolution)
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
                flat: true
                textRole: "text"
                valueRole: "value"
                // iconNameRole: "icon"
                model: root.getFillModel(screen.availableFillModes)
                function indexOfFill(currentFillMode) {
                    for (var i = 0; i < model.length; i++) {
                        let v = model[i]
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
                flat: true
                textRole: "text"
                valueRole: "value"
                model: root.getRateModel(screen.rateList, screen.bestRate)
                function indexOfRate(model, currentRate) {
                    for (var i = 0; i < model.length; i++) {
                        let v = model[i]
                        if (v.value === currentRate) {
                            return i
                        }
                    }
                    return -1
                }
                currentIndex: indexOfRate(model, screen.currentRate)
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
                flat: true
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
            description: screen.maxScale >= 1.25 ? "" : qsTr("The monitor only supports 100% display scaling")
            weight: 60
            visible: !dccData.isX11
            pageType: DccObject.Editor
            page: ComboBox {
                flat: true
                textRole: "text"
                valueRole: "value"
                model: root.getScaleModel(screen.maxScale, screen.scale)
                currentIndex: root.indexOfScale(model, screen.scale)
                onActivated: {
                    screen.scale = currentValue
                }
            }
        }
    }
    DccObject {
        name: "displayScaling"
        parentName: "display"
        displayName: qsTr("Display Scaling") //"缩放"
        description: dccData.maxGlobalScale >= 1.25 ? "" : qsTr("The monitor only supports 100% display scaling")
        weight: 80
        visible: dccData.isX11
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: ComboBox {
            flat: true
            textRole: "text"
            valueRole: "value"
            model: root.getScaleModel(dccData.maxGlobalScale, dccData.globalScale)
            currentIndex: root.indexOfScale(model, dccData.globalScale)
            onActivated: {
                dccData.globalScale = currentValue
            }
        }
    }

    DccTitleObject {
        name: "displayColorTemperature"
        parentName: "display"
        displayName: qsTr("Eye Comfort")
        weight: 90
        visible: false
    }
    DccObject {
        name: "eyeComfort"
        parentName: "display"
        displayName: qsTr("Eye Comfort")
        description: qsTr("Adjust screen display to warmer colors, reducing screen blue light")
        weight: 100
        visible: false
        backgroundType: DccObject.Normal
        pageType: DccObject.Editor
        page: Switch {}
    }
    DccObject {
        name: "eyeComfortGroup"
        parentName: "display"
        weight: 110
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
                flat: true
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
