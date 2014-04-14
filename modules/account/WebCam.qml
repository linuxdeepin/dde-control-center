import QtQuick 2.1
import QtMultimedia 5.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Api.Graphic 1.0

Item {
    id: root
    property bool running: false
    property int verticalPadding: 8

    signal avatarPictured (url path)

    Graphic { id: dbus_graphic }

    onRunningChanged: {
        if (running) {
            camera.start()
        } else {
            camera.stop()
        }
    }

    function urlToPath(url) {
        return url.toString().substring(7)
    }

    Camera {
        id: camera

        captureMode: Camera.CaptureStillImage
        property url imageSavedPath: ""

        imageCapture {
            /* resolution: Qt.size(320, 240) */

            onImageCaptured: {
                avatar_preview.source = preview
                avatar_preview.visible = true
            }

            onImageSaved: {
                imageSavedPath = path
            }
        }

        function snapshot() {
            imageCapture.capture()
        }

        Component.onCompleted: {
            if (availability != Camera.Available) {
                warning.visible = true
                video_output_box.visible = false
                mask.visible = false
                slider.visible = false
                snapshot_button.visible = false
                confirm_button.visible = false
            }
        }
    }

    Rectangle {
        id: video_output_box

        width: 120
        height: 120
        clip: true

        transform: Rotation {
            origin.x: 61
            origin.y: 60
            axis { x: 0; y: 1; z: 0 }
            angle: 180
        }

        VideoOutput {
            id: video_output
            width: 240
            height: 120
            source: camera

            fillMode: VideoOutput.PreserveAspectFit
            anchors.centerIn: parent
        }

        Image {
            id: avatar_preview
            width: 240
            height: 120

            scale: video_output.scale

            visible: false
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
        }

        anchors.fill: mask
    }


    Image {
        id: mask
        source: "images/mask.png"

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    SliderWithButtons {
        id: slider

        onPercentageChanged: {
            video_output.scale = percentage + 1
        }

        anchors.top: mask.bottom
        anchors.topMargin: root.verticalPadding
        anchors.horizontalCenter: parent.horizontalCenter
    }

    DTextButton {
        id: snapshot_button
        /* text: dsTr("Snapshot") */
        text: "Snapshot"
        property bool textIsSnapshotFlag: true

        anchors.top: slider.bottom
        anchors.topMargin: root.verticalPadding * 2
        anchors.right: confirm_button.left
        anchors.rightMargin: root.verticalPadding

        onClicked: {
            if (textIsSnapshotFlag) {
                camera.snapshot()
                text = "Resnapshot"
                textIsSnapshotFlag = false
            } else {
                avatar_preview.visible = false
                text = "Snapthot"
                textIsSnapshotFlag = true
            }
        }
    }

    DTextButton {
        id: confirm_button
        /* text: dsTr("Confirm") */
        text: "Confirm"

        anchors.right: parent.right
        anchors.rightMargin: root.verticalPadding
        anchors.verticalCenter: snapshot_button.verticalCenter

        onClicked: {
            var imagePath = urlToPath(camera.imageSavedPath)
            dbus_graphic.ResizeImage(imagePath, imagePath, 320, 240, "jpeg")            
            var scaledSize = Math.floor(240 * video_output.scale * 120 / 240)
            dbus_graphic.FlipImageHorizontal(imagePath, imagePath, "jpeg")
            dbus_graphic.ClipImage(imagePath, imagePath, 40, 0, 280, 240, "jpeg")
            dbus_graphic.ResizeImage(imagePath, imagePath, scaledSize, scaledSize, "jpeg")
            var margins = Math.floor((scaledSize - 120) / 2)
            dbus_graphic.ClipImage(imagePath, imagePath,
                                   margins, margins,
                                   margins + 120,
                                   margins + 120,
                                   "jpeg")
            dbus_graphic.ResizeImage(imagePath, imagePath, 150, 150, "jpeg")
            root.avatarPictured(camera.imageSavedPath)
        }
    }

    Text {
        id: warning
        visible: false
        text: dsTr("No camera found.")
        color: "white"
        font.pixelSize: 15
        anchors.centerIn: parent
    }
}
