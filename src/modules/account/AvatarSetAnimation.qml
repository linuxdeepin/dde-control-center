import QtQuick 2.1

ParallelAnimation {
    id: animation

    property var destination: round_image.parent.mapToItem(component_bg, round_image.x + 15, round_image.y)

    property Item target: round_image
    property point startPoint: Qt.point(target.x, target.y)
    property point endPoint: Qt.point(destination.x, destination.y)
    property int startRoundRadius: target.roundRadius
    property int endRoundRadius: round_image.roundRadius

    NumberAnimation {
        target: animation.target
        duration: 500
        easing.type: Easing.InQuad

        properties: "roundRadius"
        from: animation.startRoundRadius
        to: animation.endRoundRadius
    }

    PathAnimation {
        target: animation.target
        duration: 500
        easing.type: Easing.InQuad

        path: Path {
            startX: animation.startPoint.x
            startY: animation.startPoint.y

            PathCubic {
                x: animation.endPoint.x; y: animation.endPoint.y
                relativeControl1X: -10; relativeControl1Y: -30
                relativeControl2X: 10; relativeControl2Y: -20
            }
        }
    }

    onStopped: {
        round_image.imageSource = target.imageSource
        target.destroy()
    }
}
