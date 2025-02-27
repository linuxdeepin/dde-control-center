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
    ListModel {
        id: fillModel
    }
    function getFillModel(availableFillModes) {
        fillModel.clear()
        for (let fillmode of availableFillModes) {
            switch (fillmode) {
            case "None":
                fillModel.append({
                                     "text": qsTr("Default"),
                                     "icon": "DisplayDefault",
                                     "value": fillmode
                                 })
                break
            case "Full aspect":
                fillModel.append({
                                     "text": qsTr("Fit"),
                                     "icon": "DisplayFit",
                                     "value": fillmode
                                 })
                break
            case "Full":
                fillModel.append({
                                     "text": qsTr("Stretch"),
                                     "icon": "DisplayStretch",
                                     "value": fillmode
                                 })
                break
            case "Center":
                fillModel.append({
                                     "text": qsTr("Center"),
                                     "icon": "DisplayCenter",
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
    function getQtScreen(screen) {
        for (var s of Qt.application.screens) {
            if (s.virtualX === screen.x && s.virtualY === screen.y && s.width === screen.currentResolution.width && s.height === screen.currentResolution.height) {
                return s
            }
        }
        return null
    }
    Connections {
        target: dccData
        function onVirtualScreensChanged() {
            if (!dccData.virtualScreens.includes(screen)) {
                screen = dccData.virtualScreens[0]
            }
        }
    }
    Component {
        id: indicator
        ScreenIndicator {}
    }
    Component {
        id: recognize
        ScreenRecognize {}
    }
    DccTitleObject {
        name: "multipleDisplays"
        parentName: "display"
        displayName: qsTr("Multiple Displays Settings")
        weight: 10
        visible: dccData.screens.length > 1
    }
    DccObject {
        id: monitorControl
        property bool effective: false

        name: "monitorControl"
        parentName: "display"
        weight: 20
        visible: dccData.screens.length > 1
        backgroundType: DccObject.Normal
        pageType: DccObject.Item
        page: DccGroupView {
            isGroup: false
        }
        DccObject {
            name: "monitorsGround"
            parentName: "monitorControl"
            weight: 10
            enabled: dccData.virtualScreens.length > 1
            pageType: DccObject.Item
            page: Item {
                id: monitorsGround
                property real translationX: 100
                property real translationY: 20
                property real scale: 0.1

                implicitHeight: 240
                clip: true
                onWidthChanged: monitorRepeater.adjustAll()
                Timer {
                    interval: 2000
                    running: monitorControl.effective
                    repeat: false
                    onTriggered: {
                        var listItems = []
                        for (var i = 0; i < monitorRepeater.count; i++) {
                            var otherItem = monitorRepeater.itemAt(i)
                            listItems.push(otherItem)
                        }
                        dccData.applySettings(listItems, monitorsGround.scale)
                        monitorControl.effective = false
                    }
                }
                Repeater {
                    // 复制模式时示意图后面的框，放该位置是为了层级在所有示意图后面
                    model: dccData.virtualScreens
                    delegate: Loader {
                        active: screen.screenItems.length > 1
                        anchors.fill: monitorRepeater.itemAt(index)
                        z: -1
                        sourceComponent: Item {
                            property var screen: model.modelData
                            property D.Palette backgroundColor: D.Palette {
                                normal: Qt.rgba(1, 1, 1, 1)
                                normalDark: Qt.rgba(1, 1, 1, 0.05)
                            }
                            Repeater {
                                model: screen.screenItems.length - 1
                                delegate: Rectangle {
                                    z: -1 - index * 0.01
                                    radius: parent.width * 0.05
                                    anchors.centerIn: parent
                                    color: parent.D.ColorSelector.backgroundColor
                                    border.color: "#515151"
                                    border.width: 1
                                    width: parent.width + (radius * 2 * (index + 1))
                                    height: parent.height - (radius * 2 * (index + 1))
                                }
                            }
                        }
                    }
                }
                Repeater {
                    id: monitorRepeater
                    property bool hasMove: false
                    model: dccData.virtualScreens
                    delegate: ScreenItem {
                        screen: model.modelData
                        translationX: monitorsGround.translationX
                        translationY: monitorsGround.translationY
                        scale: monitorsGround.scale
                        selected: root.screen === screen
                        onPressed: monitorRepeater.pressedItem(this)
                        onPositionChanged: monitorRepeater.positionChangedItem(this)
                        onReleased: monitorRepeater.releasedItem(this)
                        onUpdatePosition: {
                            x = screen.x * scale + translationX
                            y = screen.y * scale + translationY
                            width = screen.width * scale
                            height = screen.height * scale
                            monitorRepeater.adjustAll()
                        }
                    }
                    onCountChanged: adjustAll()
                    Component.onCompleted: adjustAll()

                    function pressedItem(item) {
                        hasMove = false
                        root.screen = item.screen
                        if (dccData.isX11) {
                            indicator.createObject(this, {
                                                       "screen": getQtScreen(item.screen)
                                                   }).show()
                        }
                    }
                    function positionChangedItem(item) {
                        monitorControl.effective = false
                        hasMove = true
                        var listItems = []
                        // 吸附距离
                        for (var i = 0; i < monitorRepeater.count; i++) {
                            var otherItem = monitorRepeater.itemAt(i)
                            listItems.push(otherItem)
                        }
                        dccData.adsorptionScreen(listItems, item, monitorsGround.scale)
                    }
                    function releasedItem(item) {
                        if (!hasMove) {
                            return
                        }
                        hasMove = false

                        var listItems = []
                        for (var i = 0; i < monitorRepeater.count; i++) {
                            var otherItem = monitorRepeater.itemAt(i)
                            listItems.push(otherItem)
                        }
                        dccData.executemultiScreenAlgo(listItems, item, monitorsGround.scale)
                        adjustAll()
                        monitorControl.effective = false // 重置下定时器
                        monitorControl.effective = true
                    }
                    function adjustAll() {
                        if (monitorRepeater.count === 0 || monitorsGround.scale === 0 || monitorsGround.width === 0 || monitorsGround.height === 0) {
                            return
                        }
                        var firstItem = monitorRepeater.itemAt(0)

                        var gx = (firstItem.x - monitorsGround.translationX) / monitorsGround.scale
                        var gy = (firstItem.y - monitorsGround.translationY) / monitorsGround.scale
                        var gx2 = firstItem.width / monitorsGround.scale + gx
                        var gy2 = firstItem.height / monitorsGround.scale + gy
                        var itemMaxH = gy2 - gy
                        for (var i = 1; i < monitorRepeater.count; i++) {
                            let otherItem = monitorRepeater.itemAt(i)
                            var ox = (otherItem.x - monitorsGround.translationX) / monitorsGround.scale
                            var oy = (otherItem.y - monitorsGround.translationY) / monitorsGround.scale
                            var ox2 = otherItem.width / monitorsGround.scale + ox
                            var oy2 = otherItem.height / monitorsGround.scale + oy
                            if (gx > ox) {
                                gx = ox
                            }
                            if (gy > oy) {
                                gy = oy
                            }
                            if (gx2 < ox2) {
                                gx2 = ox2
                            }
                            if (gy2 < oy2) {
                                gy2 = oy2
                            }
                            if (itemMaxH < oy2 - oy) {
                                itemMaxH = oy2 - oy
                            }
                        }
                        if (gx2 === gx || gy2 === gy) {
                            return
                        }
                        var xScale = (monitorsGround.width) / ((gx2 - gx) * 1.2)
                        var yScale = (monitorsGround.height) / ((gy2 - gy) * 1.2)
                        var gScale = xScale < yScale ? xScale : yScale
                        let tX = (monitorsGround.width - (gx2 - gx) * gScale) * 0.5
                        let tY = (monitorsGround.height - (gy2 - gy) * gScale) * 0.5 // +((gy2-gy)*0.1)*gScale
                        for (var j = 0; j < monitorRepeater.count; j++) {
                            let item = monitorRepeater.itemAt(j)
                            item.x = ((item.x - monitorsGround.translationX) / monitorsGround.scale - gx) * gScale + tX
                            item.y = ((item.y - monitorsGround.translationY) / monitorsGround.scale - gy) * gScale + tY
                            item.width = item.width / monitorsGround.scale * gScale
                            item.height = item.height / monitorsGround.scale * gScale
                        }
                        monitorsGround.scale = gScale
                        monitorsGround.translationX = tX
                        monitorsGround.translationY = tY
                    }
                }
            }
        }
        DccObject {
            name: "identify"
            parentName: "monitorControl"
            displayName: qsTr("Identify")
            weight: 20
            visible: dccData.virtualScreens.length > 1 || (dccData.virtualScreens.length === 1 && dccData.virtualScreens[0].screenItems.length > 1)
            pageType: DccObject.Item
            page: Item {
                implicitHeight: identifyBut.implicitHeight + 10
                DccLabel {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.rightMargin: identifyBut.width + 5
                    anchors.leftMargin: identifyBut.width + 5
                    visible: monitorControl.effective
                    text: qsTr("Screen rearrangement will take effect in %1s after changes").arg(2)
                    clip: true
                }
                Button {
                    id: identifyBut
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 5
                    text: dccObj.displayName
                    onClicked: {
                        // if (!dccData.isX11) {
                        //     return
                        // }
                        for (var i = 0; i < dccData.virtualScreens.length; i++) {
                            var item = dccData.virtualScreens[i]
                            recognize.createObject(this, {
                                                       "screen": getQtScreen(item),
                                                       "name": item.name
                                                   }).show()
                        }
                    }
                }
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
        visible: dccData.virtualScreens.length > 1
        pageType: DccObject.Item
        Component {
            id: itmeoutDialog
            TimeoutDialog {}
        }

        page: ScreenTab {
            model: dccData.virtualScreens
            screen: root.screen
            onScreenClicked: function (screen) {
                if (screen && screen !== root.screen) {
                    root.screen = screen
                    if (dccData.isX11) {
                        indicator.createObject(this, {
                                                   "screen": getQtScreen(root.screen)
                                               }).show()
                    }
                }
            }
        }
    }
    component BrightnessComponent: RowLayout {
        property var screenItem
        Label {
            Layout.alignment: Qt.AlignVCenter
            text: Math.round(brightnessSlider.value * 100) + "%"
        }
        D.DciIcon {
            Layout.alignment: Qt.AlignVCenter
            name: "dcc_brightnesslow"
            palette: D.DTK.makeIconPalette(parent.palette)
        }
        Slider {
            id: brightnessSlider
            implicitHeight: 24
            Layout.alignment: Qt.AlignVCenter
            highlightedPassedGroove: true
            from: 0.1
            to: 1
            stepSize: 0.01
            value: screenItem.brightness
            onValueChanged: {
                if (screenItem.brightness !== value) {
                    screenItem.brightness = value
                }
            }
        }
        D.DciIcon {
            Layout.alignment: Qt.AlignVCenter
            name: "dcc_brightnesshigh"
            palette: D.DTK.makeIconPalette(parent.palette)
        }
    }
    DccObject {
        name: "brightnessGroup"
        parentName: "display"
        displayName: qsTr("Brightness")
        weight: 60
        visible: screen.screenItems.length > 1
        pageType: DccObject.Item
        page: DccGroupView {}
        DccObject {
            name: "brightness"
            parentName: "display/brightnessGroup"
            displayName: qsTr("Brightness")
            weight: 10
            pageType: DccObject.Editor
            page: Item {}
        }
        DccRepeater {
            model: screen.screenItems
            delegate: DccObject {
                name: "brightness" + index
                parentName: "display/brightnessGroup"
                weight: 20 + index
                displayName: screen.screenItems[index].name
                pageType: DccObject.Editor
                page: BrightnessComponent {
                    screenItem: screen.screenItems[index]
                }
            }
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
            visible: screen.screenItems.length === 1
            pageType: DccObject.Editor
            page: BrightnessComponent {
                screenItem: screen.screenItems[0]
            }
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
            visible: screen.availableFillModes.length > 0
            pageType: DccObject.Editor
            page: D.ComboBox {
                id: control
                flat: true
                textRole: "text"
                valueRole: "value"
                iconNameRole: "icon"
                model: root.getFillModel(screen.availableFillModes)
                function indexOfFill(model, currentFill) {
                    for (var i = 0; i < model.count; i++) {
                        let v = model.get(i)
                        if (v.value === currentFill) {
                            return i
                        }
                    }
                    return -1
                }
                currentIndex: indexOfFill(this.model, screen.currentFillMode)
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
            visible: !dccData.isX11 || dccData.virtualScreens.length === 1
            pageType: DccObject.Editor
            page: ComboBox {
                flat: true
                textRole: "text"
                valueRole: "value"
                model: dccData.isX11 ? root.getScaleModel(dccData.maxGlobalScale, dccData.globalScale) : root.getScaleModel(screen.maxScale, screen.scale)
                currentIndex: dccData.isX11 ? root.indexOfScale(model, dccData.globalScale) : root.indexOfScale(model, screen.scale)
                onActivated: {
                    if (dccData.isX11) {
                        dccData.globalScale = currentValue
                    } else {
                        screen.scale = currentValue
                    }
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
        visible: dccData.isX11 && dccData.virtualScreens.length > 1
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
        page: Switch {
            checked: dccData.colorTemperatureEnabled
            onClicked: dccData.colorTemperatureEnabled = checked
        }
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
                currentIndex: dccData.colorTemperatureMode
                onActivated: dccData.colorTemperatureMode = currentIndex
            }
        }
        DccObject {
            name: "timeFrame"
            parentName: "display/eyeComfortGroup"
            weight: 20
            visible: dccData.colorTemperatureMode === 2
            pageType: DccObject.Editor
            page: RowLayout {
                Label {
                    text: qsTr("from")
                }
                DccTimeRange {
                    hour: dccData.customColorTempTimePeriod.split("-")[0].split(":")[0]
                    minute: dccData.customColorTempTimePeriod.split("-")[0].split(":")[1]
                    onTimeChanged: dccData.customColorTempTimePeriod = timeString + dccData.customColorTempTimePeriod.substring(5, 11)
                }
                Label {
                    text: qsTr("to")
                }
                DccTimeRange {
                    hour: dccData.customColorTempTimePeriod.split("-")[1].split(":")[0]
                    minute: dccData.customColorTempTimePeriod.split("-")[1].split(":")[1]
                    onTimeChanged: dccData.customColorTempTimePeriod = dccData.customColorTempTimePeriod.substring(0, 6) + timeString
                }
            }
        }
        DccObject {
            name: "colorTemperature"
            parentName: "display/eyeComfortGroup"
            displayName: qsTr("Color Temperature")
            weight: 30
            pageType: DccObject.Editor
            page: RowLayout {
                property var screenItem
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    text: Math.round(colorTemperatureSlider.value) + "%"
                }
                D.DciIcon {
                    Layout.alignment: Qt.AlignVCenter
                    name: "cool_colour"
                }
                Slider {
                    id: colorTemperatureSlider
                    implicitHeight: 24
                    Layout.alignment: Qt.AlignVCenter
                    from: 0
                    to: 100
                    stepSize: 1
                    value: dccData.colorTemperature
                    onValueChanged: {
                        dccData.colorTemperature = value
                    }
                }
                D.DciIcon {
                    Layout.alignment: Qt.AlignVCenter
                    name: "warm_colour"
                }
            }
        }
    }
}
