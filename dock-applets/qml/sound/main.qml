import QtQuick 2.0
import QtQuick.Window 2.1
import Deepin.DockApplet 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Audio 1.0
import "../widgets"

DockApplet{
    title: "Sound"
    appid: "AppletSound"
    icon: iconPath
    property url iconPath: getIconPath()

    property int xEdgePadding: 0
    property int titleSpacing: 10
    property int rootWidth: 250
    property int wheelStep: 5

    property var audioId: Audio {}
    property var defaultSink: AudioSink{ path: audioId.GetDefaultSink() }

    function getIconPath(){
        if(typeof(defaultSink.volume) != "undefined"){
            var step = parseInt(defaultSink.volume/10) * 10
            if(step >= 100){
                step = 100
            }
        }
        else{
            step = 100
        }
        return getIconUrl("sound/sound_%1.png".arg(step))
    }

    onActivate:{
        showSound()
    }

    onMousewheel: {
        if (angleDelta > 0){
            if(defaultSink.volume <= (100 - wheelStep)){
                defaultSink.SetSinkVolume(defaultSink.volume + wheelStep)
            }
            else{
                defaultSink.SetSinkVolume(100)
            }
        }
        else if(angleDelta < 0){
            if(defaultSink.volume >= wheelStep){
                defaultSink.SetSinkVolume(defaultSink.volume - wheelStep)
            }
            else{
                defaultSink.SetSinkVolume(0)
            }
        }
    }

    function showSound(){
        dbusControlCenter.ShowModule("sound")
    }

    function hideSound(){
        set_hide_applet("sound")
    }

    menu: Menu{
        Component.onCompleted: {
            addItem("_Run", showSound);
            addItem("_Undock", hideSound);
        }
    }

    window: DockQuickWindow {
        id: root
        width: rootWidth
        height: content.height
        color: "transparent"

        Connections{
            target: defaultSink
            onVolumeChanged: {
                if(!soundSlider.pressed && !soundSlider.hovered){
                    soundSlider.value = defaultSink.volume
                }
            }
        }

        Item {
            width: parent.width
            height: content.height

            Column {
                id: content
                width: parent.width
                Item {
                    height: 30
                    width: parent.width

                    DssH2 {
                        id: allSoundLabel
                        anchors.left: parent.left
                        anchors.leftMargin: xEdgePadding
                        text: dsTr("Device")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        height: 2
                        width: parent.width - allSoundLabel.width - xEdgePadding * 2 - titleSpacing
                        anchors.right: parent.right
                        anchors.rightMargin: xEdgePadding
                        anchors.verticalCenter: parent.verticalCenter
                        opacity: 0.1
                    }
                }

                Item {
                    height: 40
                    width: parent.width

                    Image {
                        id: soundImage
                        width: 24
                        height: 24
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        source: "images/sound-indicator.png"
                        anchors.verticalCenter: parent.verticalCenter
                    }


                    WhiteSlider{
                        id: soundSlider
                        width: parent.width - soundImage.width - 16 - xEdgePadding * 2 - titleSpacing
                        anchors.right: parent.right
                        anchors.rightMargin: xEdgePadding
                        anchors.verticalCenter: parent.verticalCenter
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1

                        onValueChanged: {
                            if(pressed || hovered){
                                defaultSink.SetSinkVolume(value)
                            }
                        }

                        Timer{
                            running: true
                            interval: 200
                            onTriggered: {
                                if(!pressed && !hovered){
                                    soundSlider.value = defaultSink.volume
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 10
                    width: parent.width
                    visible: false
                }

                Item {
                    id: appSoundTitleBox
                    height: 30
                    width: parent.width
                    visible: false

                    DssH2 {
                        id: appLabel
                        anchors.left: parent.left
                        anchors.leftMargin: xEdgePadding
                        text: dsTr("Applications")
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        height: 2
                        width: parent.width - appLabel.width - xEdgePadding * 2 - titleSpacing
                        anchors.right: parent.right
                        anchors.rightMargin: xEdgePadding
                        anchors.verticalCenter: parent.verticalCenter
                        opacity: 0.1
                    }
                }

                Item {
                    width: parent.width
                    height: childrenRect.height
                    visible: false

                    ListView {
                        width: parent.width
                        height: childrenRect.height
                        model: 2
                        delegate: Item {
                            height: 32
                            width: ListView.view.width

                            Item {
                                height: 20
                                width: parent.width
                                anchors.verticalCenter: parent.verticalCenter

                                Item {
                                    id: appIconBox
                                    width: 60
                                    height: parent.height
                                    Image {
                                        height: parent.height
                                        width: parent.height
                                        anchors.left: parent.left
                                        anchors.leftMargin: 16
                                        source: "images/app.png"
                                    }
                                }

                                Image {
                                    id: soundIndicator
                                    height: parent.height
                                    width: parent.height
                                    anchors.left: appIconBox.right
                                    source: "images/sound-indicator.png"
                                }


                                WhiteSlider{
                                    id: appSlider
                                    width: parent.width - appIconBox.width - soundIndicator.width - anchors.leftMargin
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: soundIndicator.right
                                    anchors.leftMargin: 10
                                    minimumValue: 0
                                    maximumValue: 150
                                    stepSize: 1
                                }

                            }
                        }
                    }
                }
            }
        }
    }
}
