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
    property var meterSourceId: AudioMeter { path: currentSource.GetMeter() }
    property var listModelComponent: DListModelComponent {}

    Component {
        id: sinkComponent
        AudioSink {}
    }

    Component {
        id: sourceComponent
        AudioSource {}
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

    property var currentSink: {
        for (var i = 0; i < allSinks.length; i++) {
            if (allSinks[i].name == audioId.defaultSink) {
                return allSinks[i]
            }
        }
    }
    
    property var currentSource: {
        for (var i = 0; i < allSources.length; i++) {
            if (allSources[i].name == audioId.defaultSource) {
                return allSources[i]
            }
        }
    }
    
    Component.onCompleted: {
        if (dsslocale.lang == "zh") {
            leftWidth = 80
        }
    }
    
    Timer {
        id: tick_timer
        running: true
        repeat: true
        interval: 1000
        onTriggered: {
            meterSourceId.Tick()
        }
    }

    Column {
        id: titleColumn
        width: parent.width
        height: childrenRect.height
        z: 1

        DssTitle {
            text: modulesId.moduleLocaleNames["sound"]

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
            x: 0

            Behavior on y{
                PropertyAnimation {duration: 200}
            }

            DBaseLine {
                height: 38
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Speaker")
                    color: titleColor
                }

                rightLoader.sourceComponent: Component{
                    DSwitchButton {
                        checked: !currentSink.mute
                        onClicked: {
                            currentSink.SetMute(!checked)
                        }
                    }
                }
            }

            Column {
                id: outputColumn
                width: parent.width
                height: currentSink.mute ? 0 : childrenRect.height
                visible: height != 0

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
                        init: currentSink.volume * 100
                        
                        onValueChanged:{
                            currentSink.SetVolume(value / 100, false)
                        }
                        
                        onValueConfirmed: {
                            currentSink.SetVolume(value / 100, true)
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
                                    output_volume_slider.setValue(currentSink.volume * 100)
                                }
                            }
                        }

                        Connections {
                            target: soundModule

                            onCurrentSinkChanged: {
                                output_volume_slider.setValue(currentSink.volume * 100)
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
                        isBalance: true

                        onValueChanged:{
                            currentSink.SetBalance(value, false)
                        }
                        
                        onValueConfirmed: {
                            currentSink.SetBalance(value, true)
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
                            currentSource.SetMute(!checked)
                        }
                    }
                }
            }

            DSeparatorHorizontal {}

            Column {
                id: inputColumn
                width: parent.width
                height: currentSource.mute ? 0 : childrenRect.height
                visible: height != 0

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
                        init: currentSource.volume * 100

                        onValueChanged:{
							currentSource.SetVolume(value / 100, false)
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
                                    input_volume_slider.setValue(currentSource.volume * 100)
                                }
                            }
                        }
                        
                        Connections {
                            target: soundModule

                            onCurrentSourceChanged: {
                                input_volume_slider.setValue(currentSource.volume * 100)
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
                        init: 0
                        value: meterSourceId.volume
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
                        text: dsTr("No port exists in this device.")
                    }
                }

                ListView {
                    id: outputPortList
                    width: parent.width
                    height: childrenRect.height
                    visible: count != 0

                    model: {
                        var outputPortListModel = listModelComponent.createObject(outputPortList, {})
                        var ports = currentSink.ports
                        for(var i=0; i<ports.length; i++){
                            outputPortListModel.append({
                                                           "item_id": ports[i][0],
                                                           "item_name": ports[i][1]
                                                       })
                        }
                        return outputPortListModel
                    }

                    delegate: SelectItem {
                        totalItemNumber: outputPortList.count
                        selectItemId: currentSink.activePort[0]

                        onSelectAction: {
                            currentSink.SetPort(itemId)
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

                    property string selectItemId: audioId.defaultSink

                    model: {
                        var outputDeviceListModel = listModelComponent.createObject(outputDeviceList, {})
                        for(var i=0; i<allSinks.length; i++){
                            outputDeviceListModel.append({
                                                             "item_id": allSinks[i].name,
                                                             "item_name": allSinks[i].description
                                                         })
                        }
                        return outputDeviceListModel
                    }

                    delegate: SelectItem{
                        totalItemNumber: outputDeviceList.count
                        selectItemId: String(outputDeviceList.selectItemId)

                        onSelectAction: {
                            audioId.SetDefaultSink(itemId)
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
                        text: dsTr("No port exists in this device.")
                    }
                }

                ListView {
                    id: inputPortList
                    width: parent.width
                    height: childrenRect.height
                    visible: count != 0

                    model: {
                        var inputPortListModel = listModelComponent.createObject(inputPortList, {})
                        var ports = currentSource.ports
                        for(var i=0; i<ports.length; i++){
                            inputPortListModel.append({
                                                          "item_id": ports[i][0],
                                                          "item_name": ports[i][1]
                                                      })
                        }
                        return inputPortListModel
                    }

                    delegate: SelectItem {
                        totalItemNumber: inputPortList.count
                        selectItemId: currentSource.activePort[0]

                        onSelectAction: {
                            currentSource.SetPort(itemId)
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

                    property string selectItemId: audioId.defaultSource

                    model: {
                        var inputDeviceListModel = listModelComponent.createObject(inputDeviceList, {})
                        for(var i=0; i<allSources.length; i++){
                            inputDeviceListModel.append({
                                                            "item_id": allSources[i].name,
                                                            "item_name": allSources[i].description
                                                        })
                        }
                        return inputDeviceListModel
                    }

                    delegate: SelectItem{
                        totalItemNumber: inputDeviceList.count
                        selectItemId: String(inputDeviceList.selectItemId)

                        onSelectAction: {
                            audioId.SetDefaultSource(itemId)
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
