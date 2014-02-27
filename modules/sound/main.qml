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
    property int sliderWidth: 170
    property int leftWidth: 100
    property color titleColor: "#ffffff"

    property var audioId: Audio {}
    property var listModelComponent: DListModelComponent {}

    property var currentSink: inputDeviceList.currentItem.itemObj
    property var currentSource: outputDeviceList.currentItem.itemObj
    
    Component.onCompleted: {
        if (dsslocale.lang == "zh") {
            leftWidth = 80
        }
    }

    Component {
        id: sinkComponent
        AudioSink {}
    }

    Component {
        id: sourceComponent
        AudioSource {}
    }

    Column {
        id: titleColumn
        width: parent.width
        height: childrenRect.height
        z: 1

        DssTitle {
            text: dsTr("Sound")
        }

        DSeparatorHorizontal{}
    }
    
    Column {
        id: normalSettings
        width: parent.width
        height: myRealHeight
        x: 0
        y: link_button_column.isAdvanced ? titleColumn.height - myRealHeight : titleColumn.height
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
                rightLoader.sourceComponent: DSliderRect {
                    width: sliderWidth
                    leftLabel: dsTr("-")
                    rightLabel: dsTr("+")

                    value: currentSink.volume/100
                    onValueChanged: {
                        currentSink.volume = parseInt(value * 100)
                    }
                }
            }

            DBaseLine {
                height: contentHeight
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: LeftTitle {
                    text: dsTr("Balance")
                }
                rightLoader.sourceComponent: DSliderRect {
                    width: sliderWidth
                    leftLabel: dsTr("Left")
                    rightLabel: dsTr("Right")

                    value: (currentSink.balance+1)/2
                    onValueChanged: {
                        currentSink.balance = value * 2 - 1
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
                rightLoader.sourceComponent: DSliderRect {
                    width: sliderWidth
                    leftLabel: dsTr("-")
                    rightLabel: dsTr("+")

                    value: currentSource.volume/100
                    onValueChanged: {
                        currentSource.volume = parseInt(value * 100)
                    }
                }
            }

            DBaseLine {
                height: contentHeight
                leftMargin: contentLeftMargin
                leftLoader.sourceComponent: LeftTitle {
                    text: dsTr("Input Level")
                }
                rightLoader.sourceComponent: DSliderRect {
                    width: sliderWidth

                    value: (currentSource.balance+1)/2
                    onValueChanged: {
                        currentSource.balance = value * 2 - 1
                    }
                }
            }

            DSeparatorHorizontal {}
        }
    }

    Column {
        id: advancedSettings
        anchors.top: normalSettings.bottom
        width: parent.width
        height: link_button_column.isAdvanced ? myRealHeight : 0
        property int myRealHeight: childrenRect.height
        clip: true

        Behavior on height {
            PropertyAnimation {duration: 200}
        }

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Output")
                color: titleColor
            }
        }

        DSeparatorHorizontal {}

        DBaseLine{
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Output Port")
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: childrenRect.height
            color: dconstants.contentBgColor

            ListView {
                id: outputPortList
                focus: true

                property int selectItemId: currentSink.activePort

                model: {
                    var outputPortListModel = listModelComponent.createObject(outputPortList, {})
                    var ports = currentSink.ports
                    outputPortList.height = ports.length * lineHeight
                    for(var i=0; i<ports.length; i++){
                        var portObj = {}
                        portObj['id'] = ports[i][0]
                        portObj['name'] = ports[i][1]
                        portObj['choose'] = ports[i][2]

                        outputPortListModel.append({
                            "item_id": i,
                            "item_name": portObj['name']
                        })
                    }
                    return outputPortListModel
                }

                delegate: SelectItem {
                    totalItemNumber: outputPortList.count
                    selectItemId: String(outputPortList.selectItemId)
                }
            }
        }

        DSeparatorHorizontal{}

        DBaseLine{
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Choose audio output device")
            }
        }

        DSeparatorHorizontal {}

        ListView{
            id: inputDeviceList
            property int lineHeight: 30

            width: parent.width

            model: {
                var inputDeviceListModel = listModelComponent.createObject(inputDeviceList, {})
                var sinkPaths = audioId.GetSinks()
                inputDeviceList.height = sinkPaths.length * lineHeight
                for(var i=0; i<sinkPaths.length; i++){
                    var sinkObj = sinkComponent.createObject(soundModule, { path: sinkPaths[i] })
                    inputDeviceListModel.append({
                                                    "name": sinkObj.description,
                                                    "obj": sinkObj
                                                })
                }
                return inputDeviceListModel
            }

            delegate: ChooseItem {}
        }


        DSeparatorHorizontal{}

        DBaseLine {
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Input")
                color: titleColor
            }
        }

        DSeparatorHorizontal {}

        DBaseLine{
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Input Port")
            }
        }

        DSeparatorHorizontal {}

        Rectangle {
            width: parent.width
            height: childrenRect.height
            color: dconstants.contentBgColor

            ListView {
                id: inputPortList
                focus: true
                currentIndex: currentSource.activePort

                model: {
                    var inputPortListModel = listModelComponent.createObject(inputPortList, {})
                    var ports = currentSource.ports
                    inputPortList.height = ports.length * lineHeight
                    for(var i=0; i<ports.length; i++){
                        var portObj = {}
                        portObj['id'] = ports[i][0]
                        portObj['name'] = ports[i][1]
                        portObj['choose'] = ports[i][2]

                        inputPortListModel.append({
                                                      "name": ports[i][1],
                                                      "obj": portObj
                                                  })
                    }
                    return inputPortListModel
                }

                delegate: ChooseItem {}
            }
        }

        DSeparatorHorizontal {}

        DBaseLine{
            leftMargin: contentLeftMargin
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Choose audio input device")
            }
        }
        DSeparatorHorizontal {}

        ListView{
            id: outputDeviceList
            property int lineHeight: 30

            width: parent.width
            height: model.count * lineHeight
            currentIndex: 1

            model: {
                var outputDeviceListModel = listModelComponent.createObject(outputDeviceList, {})
                var sourcePaths = audioId.GetSources()
                outputDeviceList.height = sourcePaths.length * lineHeight
                for(var i=0; i<sourcePaths.length; i++){
                    var sourceObj = sourceComponent.createObject(soundModule, { path: sourcePaths[i] })
                    outputDeviceListModel.append({
                                                     "name": sourceObj.description,
                                                     "obj": sourceObj
                                                 })
                }
                return outputDeviceListModel
            }

            delegate: ChooseItem {}
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
            rightLoader.sourceComponent: LinkButton {
                id: link_button
                text: link_button_column.isAdvanced ? dsTr("Hide Advanced...") : dsTr("Show Advanced...")
                onClicked: {
                    link_button_column.isAdvanced = !link_button_column.isAdvanced
                }
            }
        }
    }
}
