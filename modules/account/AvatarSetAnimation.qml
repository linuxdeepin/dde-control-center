import QtQuick 2.1

PathAnimation {
    target: null
    duration: 500
    easing.type: Easing.InQuad
    
    id: animation

    /* property variant target: animation */
    property point startPoint: Qt.point(0, 0)
    property point endPoint: Qt.point(0, 0)
    property size startSize: Qt.size(0, 0)
    property size endSize: Qt.size(0, 0)
    
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

/* ParallelAnimation { */
/*     id: animation */

/*     property variant target: animation */
/*     property point startPoint: Qt.point(0, 0) */
/*     property point endPoint: Qt.point(0, 0) */
/*     property size startSize: Qt.size(0, 0) */
/*     property size endSize: Qt.size(0, 0) */

/*     NumberAnimation { */
/*         target: animation.target */
/*         duration: 500 */
/*         easing.type: Easing.InQuad */

/*         properties: "width" */
/*         from: startSize.width */
/*         to: endSize.width */
/*     } */

/*     NumberAnimation { */
/*         target: animation.target */
/*         duration: 500 */
/*         easing.type: Easing.InQuad */

/*         properties: "height" */
/*         from: startSize.height */
/*         to: endSize.height */
/*     } */

/*     PathAnimation { */
/*         target: animation.target */
/*         duration: 500 */
/*         easing.type: Easing.InQuad */

/*         path: Path { */
/*             startX: animation.startPoint.x */
/*             startY: animation.startPoint.y */

/*             PathCubic { */
/*                 x: animation.endPoint.x; y: animation.endPoint.y */
/*                 relativeControl1X: -10; relativeControl1Y: -30 */
/*                 relativeControl2X: 10; relativeControl2Y: -20 */
/*             } */
/*         } */
/*     } */
/* } */