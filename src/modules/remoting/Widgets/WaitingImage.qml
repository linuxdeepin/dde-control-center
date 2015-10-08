import QtQuick 2.2

Image {
    id: container
    visible: true
    source: "../images/waiting.png"
    RotationAnimation on rotation {
        //running: container.visible
        from: 0;
        to: 360;
        loops: Animation.Infinite;
        duration: 720
        direction: RotationAnimation.Clockwise
    }
}
