import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Audio 1.0
import "../shared/"

Item {
    id: soundModule
    anchors.fill: parent

    property int contentLeftMargin: 22
    property int lineHeight: 30
    property int contentHeight: 60
    property int sliderWidth: 171
    property int leftWidth: 100
    property color titleColor: "#ffffff"
    property int itemLabelLeftMargin: 22

    property int currrentStreamIndex

    property var audioId: Audio {}
    property var listModelComponent: DListModelComponent {}

    Component {
        id: sinkComponent
        AudioSink {}
    }

    Component {
        id: sourceComponent
        AudioSource {}
    }

    Component {
        id: applicationComponent
        AudioApplication {}
    }

    property var allSources: {
        var sourceList = new Array()
        var sourcePaths = audioId.sources
        for(var i=0; i<sourcePaths.length; i++){
            var sourceObj = sourceComponent.createObject(soundModule, { path: sourcePaths[i] })
            sourceList.push(sourceObj)
        }
        return sourceList
    }

    property var allSinks: {
        var sinkList = new Array()
        var sinkPaths = audioId.sinks
        for(var i=0; i<sinkPaths.length; i++){
            var sinkObj = sinkComponent.createObject(soundModule, { path: sinkPaths[i] })
            sinkList.push(sinkObj)
        }
        return sinkList
    }

    property var currentSink: allSinks[audioId.defaultSink]
    property var currentSource: allSources[audioId.defaultSource]
    property var currentStream: {
        var result = null

        while(!result) {
            var allApplications = currentSource.GetSourceOutputs()
            for (var i = 0; i < allApplications.length; i++) {
                var _app = applicationComponent.createObject(soundModule, {path: allApplications[i]})
                if (_app.name == 'dde daemon audio stream') {
                    result = _app
                    currrentStreamIndex = _app.index
                }
            }
            currentSource.CreateStream()
        }

        return result
    }

    Component.onCompleted: {
        if (dsslocale.lang == "zh") {
            leftWidth = 80
        }
    }

    Component.onDestruction: {
        audioId.KillSourceOutput(currrentStreamIndex)
    }

    Column {
        id: titleColumn
        width: parent.width
        height: childrenRect.height
        z: 1

        DssTitle {
            text: dsTr("Sound")

            rightLoader.sourceComponent: ResetButton {
                onClicked: audioId.Reset()
            }
        }

        DSeparatorHorizontal{}
    }

    Flickable {
        anchors.top: titleColumn.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: parent.width
        contentHeight: normalSettings.height + advancedSettings.height + link_button_column.height

        Column {
            id: normalSettings
            width: parent.width
            height: myRealHeight
            x: 0
            /* y: link_button_column.isAdvanced ? titleColumn.height - myRealHeight : titleColumn.height */
            property int myRealHeight: childrenRect.height
            clip: true

            Behavior on y{
                PropertyAnimation {duration: 200}
            }

            DBaseLine{height: 8}
            DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Speaker")
                    color: titleColor
                }

                rightLoader.sourceComponent: Component{
                    DSwitchButton {
                        checked: !currentSink.mute
                        onClicked: {
                            currentSink.mute = !checked
                        }
                    }
                }
            }

            Column {
                id: outputColumn
                width: parent.width
                height: currentSink.mute ? 0 : childrenRect.height
                clip: true

                Behavior on height {
                    NumberAnimation { duration: 100 }
                }

                DSeparatorHorizontal {}

                DBaseLine {
                    height: contentHeight
                    leftMargin: contentLeftMargin
                    leftLoader.sourceComponent: LeftTitle {
                        text: dsTr("Output Volume")
                    }
                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: output_volume_slider
                        width: sliderWidth

                        min: 0
                        max: 150
                        init: currentSink.volume

                        onValueChanged:{
                            currentSink.volume = value
                        }

                        Component.onCompleted: {
                            addRuler(0, "-")
                            addRuler(100, "")
                            addRuler(150, "+")
                        }

                        Connections {
                            target: currentSink

                            onVolumeChanged: {
                                if (!output_volume_slider.pressedFlag) {
                                    output_volume_slider.setValue(currentSink.volume)
                                }
                            }
                        }

                        Connections {
                            target: soundModule

                            onCurrentSinkChanged: {
                                output_volume_slider.setValue(currentSink.volume)
                            }
                        }
                    }
                }

                DBaseLine {
                    height: contentHeight
                    leftMargin: contentLeftMargin
                    leftLoader.sourceComponent: LeftTitle {
                        text: dsTr("Balance")
                    }
                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: output_balance_slider
                        width: sliderWidth

                        min: -1
                        max: 1
                        init: currentSink.balance
                        completeColorVisible: false
                        floatNumber: 2

                        handler.source: "images/balance.png"

                        onValueChanged:{
                            // currentSink.balance = value
                            currentSink.SetSinkBalance(value)
                        }

                        Component.onCompleted: {
                            addRuler(-1, dsTr("Left"))
                            addRuler(0, "")
                            addRuler(1, dsTr("Right"))
                        }

                        Connections {
                            target: currentSink

                            onBalanceChanged: {
                                if (currentSink.balance.toFixed(output_balance_slider.floatNumber) == 
                                    output_balance_slider.value.toFixed(output_balance_slider.floatNumber))
                                    return
                                if (!output_balance_slider.pressedFlag) {
                                    output_balance_slider.setValue(currentSink.balance)
                                }
                            }
                        }

                        Connections {
                            target: soundModule

                            onCurrentSinkChanged: {
                                output_balance_slider.setValue(currentSink.balance)
                            }
                        }
                    }
                }
            }

            DSeparatorHorizontal {}

            DBaseLine {}
            DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Microphone")
                    color: titleColor
                }

                rightLoader.sourceComponent: Component{
                    DSwitchButton {
                        checked: !currentSource.mute
                        onClicked: {
                            currentSource.mute = !checked
                        }
                    }
                }
            }

            DSeparatorHorizontal {}

            Column {
                id: inputColumn
                width: parent.width
                height: currentSource.mute ? 0 : childrenRect.height

                Behavior on height {
                    NumberAnimation { duration: 100 }
                }


                DBaseLine {
                    height: contentHeight
                    leftMargin: contentLeftMargin
                    leftLoader.sourceComponent: LeftTitle {
                        text: dsTr("Input Volume")
                    }
                    rightLoader.sourceComponent: DSliderEnhanced {
                        id: input_volume_slider
                        width: sliderWidth

                        min: 0
                        max: 150
                        init: currentSource.volume

                        onValueChanged:{
                            currentSource.volume = value
                        }

                        Component.onCompleted: {
                            addRuler(0, "-")
                            addRuler(100, "")
                            addRuler(150, "+")
                        }

                        Connections {
                            target: currentSource

                            onVolumeChanged: {
                                if (!input_volume_slider.pressedFlag) {
                                    input_volume_slider.setValue(currentSource.volume)
                                }
                            }
                        }
                        
                        Connections {
                            target: soundModule

                            onCurrentSourceChanged: {
                                input_volume_slider.setValue(currentSource.volume)
                            }
                        }
                    }
                }

                DBaseLine {
                    height: contentHeight
                    leftMargin: contentLeftMargin
                    leftLoader.sourceComponent: LeftTitle {
                        text: dsTr("Input Feedback")
                    }
                    rightLoader.sourceComponent: DSliderEnhanced {
                        width: sliderWidth

                        min: 0
                        max: 1
                        init: currentStream.inputLevel
                        value: currentStream.inputLevel
                        handlerVisible: false
                        valueDisplayVisible: false
                        showPulseGradient: true
                        clickable: false
                    }
                }

                DSeparatorHorizontal {}
            }
        }

        Column {
            id: advancedSettings
            width: parent.width
            height: link_button_column.isAdvanced ? myRealHeight : 0
            anchors.top: normalSettings.bottom
            clip: true

            property int myRealHeight: childrenRect.height

            Behavior on height {
                PropertyAnimation {duration: 200}
            }

            DBaseLine{}
            DBaseLine{
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Output Port")
                    color: titleColor
                }
            }

            DSeparatorHorizontal {}

            Rectangle {
                width: parent.width
                height: childrenRect.height
                color: dconstants.contentBgColor

                DBaseLine{
                    visible: outputPortList.count == 0
                    color: dconstants.contentBgColor
                    leftMargin: itemLabelLeftMargin
                    leftLoader.sourceComponent: DssH3{
                        text: dsTr("No port exist in this device.")
                    }
                }

                ListView {
                    id: outputPortList
                    width: parent.width
                    height: childrenRect.height
                    visible: count != 0

                    property int selectItemId: currentSink.activePort

                    model: {
                        var outputPortListModel = listModelComponent.createObject(outputPortList, {})
                        var ports = currentSink.ports
                        for(var i=0; i<ports.length; i++){
                            outputPortListModel.append({
                                                           "item_id": i,
                                                           "item_name": ports[i][1]
                                                       })
                        }
                        return outputPortListModel
                    }

                    delegate: SelectItem {
                        labelLeftMargin: itemLabelLeftMargin
                        totalItemNumber: outputPortList.count
                        selectItemId: String(outputPortList.selectItemId)

                        onSelectAction: {
                            currentSink.SetSinkPort(itemId)
                        }
                    }
                } // End of outputPortList

            }

            DSeparatorHorizontal{}

            DBaseLine{
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Output device")
                    color: titleColor
                }
            }

            DSeparatorHorizontal {}

            Rectangle {
                width: parent.width
                height: childrenRect.height
                color: dconstants.contentBgColor

                ListView{
                    id: outputDeviceList
                    width: parent.width
                    height: count * 28

                    property int selectItemId: audioId.defaultSink

                    model: {
                        var outputDeviceListModel = listModelComponent.createObject(outputDeviceList, {})
                        for(var i=0; i<allSinks.length; i++){
                            outputDeviceListModel.append({
                                                             "item_id": i,
                                                             "item_name": allSinks[i].description
                                                         })
                        }
                        return outputDeviceListModel
                    }

                    delegate: SelectItem{
                        labelLeftMargin: itemLabelLeftMargin
                        totalItemNumber: outputDeviceList.count
                        selectItemId: String(outputDeviceList.selectItemId)

                        onSelectAction: {
                            audioId.defaultSink = itemId
                        }
                    }
                } // End of inputDeviceList

            }


            DSeparatorHorizontal{}

            DBaseLine {}
            DBaseLine{
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Input Port")
                    color: titleColor
                }
            }

            DSeparatorHorizontal {}

            Rectangle {
                width: parent.width
                height: childrenRect.height
                color: dconstants.contentBgColor

                DBaseLine{
                    visible: inputPortList.count == 0
                    color: dconstants.contentBgColor
                    leftMargin: itemLabelLeftMargin
                    leftLoader.sourceComponent: DssH3{
                        text: dsTr("No port exist in this device.")
                    }
                }

                ListView {
                    id: inputPortList
                    width: parent.width
                    height: childrenRect.height
                    visible: count != 0

                    property int selectItemId: currentSource.activePort

                    model: {
                        var inputPortListModel = listModelComponent.createObject(inputPortList, {})
                        var ports = currentSource.ports
                        for(var i=0; i<ports.length; i++){
                            inputPortListModel.append({
                                                          "item_id": i,
                                                          "item_name": ports[i][1]
                                                      })
                        }
                        return inputPortListModel
                    }

                    delegate: SelectItem {
                        labelLeftMargin: itemLabelLeftMargin
                        totalItemNumber: inputPortList.count
                        selectItemId: String(inputPortList.selectItemId)

                        onSelectAction: {
                            currentSource.SetSourcePort(itemId)
                        }
                    }
                } // End of outputPortList

            }

            DSeparatorHorizontal {}

            DBaseLine{
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Input device")
                    color: titleColor
                }
            }
            DSeparatorHorizontal {}

            Rectangle {
                width: parent.width
                height: childrenRect.height
                color: dconstants.contentBgColor

                ListView{
                    id: inputDeviceList
                    width: parent.width
                    height: childrenRect.height

                    property int selectItemId: audioId.defaultSource

                    model: {
                        var inputDeviceListModel = listModelComponent.createObject(inputDeviceList, {})
                        for(var i=0; i<allSources.length; i++){
                            inputDeviceListModel.append({
                                                            "item_id": i,
                                                            "item_name": allSources[i].description
                                                        })
                        }
                        return inputDeviceListModel
                    }

                    delegate: SelectItem{
                        labelLeftMargin: itemLabelLeftMargin
                        totalItemNumber: inputDeviceList.count
                        selectItemId: String(inputDeviceList.selectItemId)

                        onSelectAction: {
                            audioId.defaultSource = itemId
                        }
                    }
                }
            }

            DSeparatorHorizontal {}
        }

        Column {
            id: link_button_column
            anchors.top: advancedSettings.bottom
            width: parent.width
            height: childrenRect.height
            property bool isAdvanced: false

            DBaseLine{
                rightLoader.sourceComponent: DTextAction {
                    id: link_button
                    text: link_button_column.isAdvanced ? dsTr("Hide Advanced...") : dsTr("Show Advanced...")
                    onClicked: {
                        link_button_column.isAdvanced = !link_button_column.isAdvanced
                    }
                }
            }
        }
    }
}
