import QtQuick 2.1
import QtMultimedia 5.0
import "../widgets"

Item {
    id: root
    width: 310
    height: 240

    property int horizontalPadding: 8

    DRadioButton {
        id: radio_button

        buttonModel: [
            {"buttonId": "recently_used", "buttonLabel": "Recently Used"},
            {"buttonId": "default", "buttonLabel": "Default"},
            {"buttonId": "webcam", "buttonLabel": "Webcam"},
        ]

        anchors.top: parent.top
        anchors.topMargin: root.horizontalPadding
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Item {
        id: avatar_default_view
    }

    Item {
        id: avatar_webcam_view

        Camera {
            id: camera

            imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash

            imageCapture {
                resolution: Qt.size(320, 240)
                onImageCaptured: {
                }
            }
        }

        VideoOutput {
            id: video_output

            width: 120
            height: 120
            source: camera
            scale: 1.0

            fillMode: VideoOutput.PreserveAspectCrop

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Image {
            id: mask
            source: "images/mask.png"

            anchors.fill: video_output
        }

        SliderWithButtons {
            id: slider

            anchors.top: mask.bottom
            anchors.topMargin: root.horizontalPadding
            anchors.horizontalCenter: parent.horizontalCenter
        }

        DTextButton {
            id: snapshot_button
            text: dsTr("Snapshot")

            anchors.top: slider.bottom
            anchors.topMargin: root.horizontalPadding * 2
            anchors.right: confirm_button.left
            anchors.rightMargin: root.horizontalPadding

            onClicked: {
                root.cancel()
            }
        }

        DTextButton {
            id: confirm_button
            text: dsTr("Confirm")

            anchors.right: parent.right
            anchors.rightMargin: root.horizontalPadding
            anchors.verticalCenter: snapshot_button.verticalCenter

            onClicked: {
                root.confirm(radio_button.currentIndex == 1)
            }
        }

        anchors.top: radio_button.bottom
        anchors.topMargin: root.horizontalPadding
        anchors.left: parent.left
        anchors.right: parent.right
    }
}