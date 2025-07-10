// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
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
            if (s.virtualX === screen.x && s.virtualY === screen.y && (Math.abs(s.width * s.devicePixelRatio - screen.currentResolution.width) < 1) && (Math.abs(s.height * s.devicePixelRatio - screen.currentResolution.height) < 1)) {
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
            id: groundObj
            function cacheImage() {
                for (var i = 0; i < dccData.virtualScreens.length; i++) {
                    var screen = dccData.virtualScreens[i]
                    DccApp.cacheImage(screen.wallpaper)
                }
            }
            Connections {
                target: dccData
                function onVirtualScreensChanged() {
                    groundObj.cacheImage()
                }
                function onWallpaperChanged() {
                    groundObj.cacheImage()
                }
            }
            name: "monitorsGround"
            parentName: "monitorControl"
            weight: 10
            enabled: dccData.virtualScreens.length > 1
            Component.onCompleted: cacheImage()
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
                    id: monitorRepeater
                    property bool hasMove: false
                    model: dccData.virtualScreens
                    delegate: ScreenItem {
                        z: selected ? 2 : 1
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
                            if (!otherItem) {
                                return
                            }
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
                implicitHeight: identifyBut.implicitHeight + 4
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
                D.Button {
                    id: identifyBut
                    property var recognizes: []
                    implicitHeight: 24
                    implicitWidth: 72
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 7
                    text: dccObj.displayName
                    function closeWindow() {
                        recognizeTimer.stop()
                        for (var obj of identifyBut.recognizes) {
                            obj.close()
                        }
                        identifyBut.recognizes = []
                    }
                    Timer {
                        id: recognizeTimer
                        repeat: false
                        interval: 5000
                        onTriggered: identifyBut.closeWindow()
                    }
                    onClicked: {
                        // if (!dccData.isX11) {
                        //     return
                        // }
                        identifyBut.closeWindow()
                        for (var i = 0; i < dccData.virtualScreens.length; i++) {
                            var item = dccData.virtualScreens[i]
                            var obj = recognize.createObject(this, {
                                                                 "screen": getQtScreen(item),
                                                                 "name": item.name
                                                             })
                            obj.show()
                            obj.escPressed.connect(identifyBut.closeWindow)
                            recognizes.push(obj)
                        }
                        recognizeTimer.restart()
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
            page: D.ComboBox {
                ListModel {
                    id: modeModel
                }
                function getModeModel(screens) {
                    modeModel.clear()
                    modeModel.append({
                                         "text": qsTr("Duplicate"),
                                         "value": "MERGE" // 1
                                     })
                    modeModel.append({
                                         "text": qsTr("Extend"),
                                         "value": "EXTEND" // 2
                                     })
                    for (let screen of screens) {
                        modeModel.append({
                                             "text": qsTr("Only on %1").arg(screen.name),
                                             "value": screen.name
                                         })
                    }
                    return modeModel
                }
                function indexOfMode(mode) {
                    for (var i = 0; i < model.count; i++) {
                        if (model.get(i).value === mode) {
                            return i
                        }
                    }
                    return -1
                }
                flat: true
                textRole: "text"
                valueRole: "value"
                model: getModeModel(dccData.screens)
                currentIndex: indexOfMode(dccData.displayMode)
                onActivated: {
                    if (dccData.displayMode !== currentValue) {
                        dccData.displayMode = currentValue
                    }
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
                    if (dccData.primaryScreen !== currentValue) {
                        dccData.primaryScreen = currentValue
                    }
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
            id: timeoutDialog
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
            font: D.DTK.fontManager.t10
            opacity: 0.5
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
                    if (screen.currentResolution === currentValue) {
                        return
                    }
                    screen.currentResolution = currentValue
                    if (dccData.isX11) {
                        timeoutDialog.createObject(this, {
                                                       "screen": getQtScreen(root.screen)
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
                ListModel {
                    id: fillmodellist
                    ListElement {
                        text: qsTr("Default")
                        icon: "DisplayDefault"
                        value: "None"
                    }
                    ListElement {
                        text: qsTr("Stretch")
                        icon: "DisplayStretch"
                        value: "Full"
                    }
                    ListElement {
                        text: qsTr("Center")
                        icon: "DisplayCenter"
                        value: "Center"
                    }
                    ListElement {
                        text: qsTr("Fit")
                        icon: "DisplayFit"
                        value: "Full aspect"
                    }
                }
                ListModel {
                    id: fillModel
                }
                function getFillModel(availableFillModes, currentFillMode) {
                    fillModel.clear()
                    for (var i = 0; i < fillmodellist.count; i++) {
                        let fillmode = fillmodellist.get(i)
                        let value = fillmode.value
                        if (value === currentFillMode || availableFillModes.indexOf(value) >= 0) {
                            fillModel.append(fillmode)
                        }
                    }
                    return fillModel
                }
                function indexOfFill(model, currentFill) {
                    for (var i = 0; i < model.count; i++) {
                        let v = model.get(i)
                        if (v.value === currentFill) {
                            return i
                        }
                    }
                    return -1
                }
                flat: true
                textRole: "text"
                valueRole: "value"
                iconNameRole: "icon"
                contentItem: D.IconLabel {
                    rightPadding: DS.Style.comboBox.spacing
                    alignment: control.horizontalAlignment
                    icon.name: (control.currentIndex >= 0 && control.iconNameRole && model.get(control.currentIndex)[control.iconNameRole] !== undefined) ? model.get(control.currentIndex)[control.iconNameRole] : null
                    icon.height: DS.Style.comboBox.iconSize
                    icon.width: DS.Style.comboBox.iconSize
                    text: control.editable ? control.editText : control.displayText
                    font: control.font
                    color: control.palette.windowText
                    spacing: DS.Style.comboBox.spacing
                }
                model: getFillModel(screen.availableFillModes, screen.currentFillMode)
                currentIndex: indexOfFill(this.model, screen.currentFillMode)
                onActivated: {
                    if (screen.currentFillMode === currentValue) {
                        return
                    }
                    screen.currentFillMode = currentValue
                    if (dccData.isX11) {
                        timeoutDialog.createObject(this, {
                                                       "screen": getQtScreen(root.screen)
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
                    if (screen.currentRate === currentValue) {
                        return
                    }
                    screen.currentRate = currentValue
                    if (dccData.isX11) {
                        timeoutDialog.createObject(this, {
                                                       "screen": getQtScreen(root.screen)
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
                    if (screen.rotate !== currentValue) {
                        screen.rotate = currentValue
                    }
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
                        if (dccData.globalScale !== currentValue) {
                            dccData.globalScale = currentValue
                        }
                    } else {
                        if (screen.scale !== currentValue) {
                            screen.scale = currentValue
                        }
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
                if (dccData.globalScale !== currentValue) {
                    dccData.globalScale = currentValue
                }
            }
        }
    }

    DccTitleObject {
        name: "displayColorTemperature"
        parentName: "display"
        displayName: qsTr("Eye Comfort")
        weight: 90
    }
    DccObject {
        name: "eyeComfort"
        parentName: "display"
        displayName: qsTr("Enable eye comfort")
        description: qsTr("Adjust screen display to warmer colors, reducing screen blue light")
        weight: 100
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
        visible: dccData.colorTemperatureEnabled
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
                    id: startTimeRange
                    hour: dccData.customColorTempTimePeriod.split("-")[0].split(":")[0]
                    minute: dccData.customColorTempTimePeriod.split("-")[0].split(":")[1]
                    onTimeChanged: dccData.customColorTempTimePeriod = startTimeRange.timeString + "-" + endTimeRange.timeString
                }
                Label {
                    text: qsTr("to")
                }
                DccTimeRange {
                    id: endTimeRange
                    hour: dccData.customColorTempTimePeriod.split("-")[1].split(":")[0]
                    minute: dccData.customColorTempTimePeriod.split("-")[1].split(":")[1]
                    onTimeChanged: dccData.customColorTempTimePeriod = startTimeRange.timeString + "-" + endTimeRange.timeString
                }
                Connections {
                    target: dccData
                    function onCustomColorTempTimePeriodChanged() {
                        var period = dccData.customColorTempTimePeriod.split("-")
                        startTimeRange.hour = period[0].split(":")[0]
                        startTimeRange.minute = period[0].split(":")[1]
                        endTimeRange.hour = period[1].split(":")[0]
                        endTimeRange.minute = period[1].split(":")[1]
                    }
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
