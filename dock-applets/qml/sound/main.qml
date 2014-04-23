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

    property int xEdgePadding: 10
    property int rootWidth: 260

    property var audioId: Audio {}
    property var defaultSink: AudioSink{ path: audioId.GetDefaultSink() }

    function getIconPath(){
        if(defaultSink.volume){
            var step = parseInt(defaultSink.volume/10) * 10
            if(step >= 100){
                step = 100
            }
        }
        else{
            step = 100
        }
        return "images/sound-%1.png".arg(step)
    }

    onActivate:{
        showSound()
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
        height: 200
        color: Qt.rgba(0, 0, 0, 0.85)

        Connections{
            target: defaultSink
            onVolumeChanged: {
                if(!soundSlider.pressed){
                    soundSlider.value = defaultSink.volume
                }
            }
        }

        Item {
            anchors.centerIn: parent
            width: parent.width - xEdgePadding * 2
            height: parent.height - xEdgePadding * 2

            Column {
                width: parent.width
                Item {
                    height: 30
                    width: parent.width

                    DssH2 {
                        id: allSoundLabel
                        anchors.left: parent.left
                        anchors.leftMargin: xEdgePadding
                        text: "全局声音"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        height: 2
                        width: parent.width - allSoundLabel.width - xEdgePadding * 3
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
                        width: parent.width - soundImage.width - 16 - xEdgePadding * 3
                        anchors.right: parent.right
                        anchors.rightMargin: xEdgePadding
                        anchors.verticalCenter: parent.verticalCenter
                        minimumValue: 0
                        maximumValue: 150
                        stepSize: 1

                        onValueChanged: {
                            if(pressed){
                                defaultSink.volume = value
                            }
                        }

                        Timer{
                            running: true
                            interval: 200
                            onTriggered: {
                                soundSlider.value = defaultSink.volume
                            }
                        }
                    }
                }

                Item {
                    height: 10
                    width: parent.width
                }

                Item {
                    height: 30
                    width: parent.width

                    DssH2 {
                        id: appLabel
                        anchors.left: parent.left
                        anchors.leftMargin: xEdgePadding
                        text: "应用程序"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        height: 2
                        width: parent.width - appLabel.width - xEdgePadding * 3
                        anchors.right: parent.right
                        anchors.rightMargin: xEdgePadding
                        anchors.verticalCenter: parent.verticalCenter
                        opacity: 0.1
                    }
                }
            }
        }
    }
}
