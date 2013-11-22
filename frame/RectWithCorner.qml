import QtQuick 2.1
import QtGraphicalEffects 1.0

Rectangle {
    id: rect
    width: rectWidth
    height: rectHeight
    color: Qt.rgba(0, 0, 0, 0)

    property bool withBlur: true
    property color fillColor: Qt.rgba(0, 0, 0, 0.7)
    property color blurColor: Qt.rgba(0, 0, 0, 1)
    property color borderColor: Qt.rgba(1, 1, 1, 0.15)

    property int blurRadius: 16
    property int blurWidth: 10
    property int borderWidth: 2
    property int rectRadius: 4
    property int rectWidth: 200
    property int rectHeight: 200

    property string cornerDirection: "down"
    property int cornerPos: rectWidth / 2
    property int cornerWidth: 24
    property int cornerHeight: 12

    Canvas {
        id: canvas
        width: rectWidth
        height: rectHeight
        
        onWidthChanged: requestPaint()        
        onHeightChanged: requestPaint()
        
        onPaint: {
            var ctx = getContext("2d")

            ctx.save()
            ctx.clearRect(0, 0, canvas.width, canvas.height)

            ctx.beginPath();

            if (cornerDirection == "down") {
                var x = blurWidth
                var y = blurWidth
                var w = rectWidth - 2 * blurWidth
                var h = rectHeight - 2 * blurWidth - cornerHeight

                ctx.moveTo(x + rectRadius, y);                 // top side
                ctx.lineTo(x + w - rectRadius, y);
                // draw top right corner
                ctx.arcTo(x + w, y, x + w, y + rectRadius, rectRadius);
                ctx.lineTo(x+w,y+h-rectRadius);    // right side
                // draw bottom right corner
                ctx.arcTo(x+w,y+h,x+w-rectRadius,y+h,rectRadius);

                if (cornerPos < x + rectRadius + cornerWidth / 2) {
                    cornerPos = x + rectRadius + cornerWidth / 2
                }
                if (cornerPos > x + w - rectRadius - cornerWidth / 2) {
                    cornerPos = x + w - rectRadius - cornerWidth / 2
                }
                ctx.lineTo(cornerPos + cornerWidth / 2, y + h) /* corner */
                ctx.lineTo(cornerPos, y + h + cornerHeight)
                ctx.lineTo(cornerPos - cornerWidth / 2, y + h)

                ctx.lineTo(x+rectRadius,y+h);              // bottom side
                // draw bottom left corner
                ctx.arcTo(x,y+h,x,y+h-rectRadius,rectRadius);
                ctx.lineTo(x,y+rectRadius);                 // left side
                // draw top left corner
                ctx.arcTo(x,y,x+rectRadius,y,rectRadius);
            } else if (cornerDirection == "up") {
                var x = blurWidth
                var y = blurWidth + cornerHeight
                var w = rectWidth - 2 * blurWidth
                var h = rectHeight - 2 * blurWidth - cornerHeight

                ctx.moveTo(x + rectRadius, y);                 // top side

                if (cornerPos < x + rectRadius + cornerWidth / 2) {
                    cornerPos = x + rectRadius + cornerWidth / 2
                }
                if (cornerPos > x + w - rectRadius - cornerWidth / 2) {
                    cornerPos = x + w - rectRadius - cornerWidth / 2
                }
                ctx.lineTo(cornerPos - cornerWidth / 2, y) /* corner */
                ctx.lineTo(cornerPos, y - cornerHeight)
                ctx.lineTo(cornerPos + cornerWidth / 2, y)

                ctx.lineTo(x + w - rectRadius, y);

                // draw top right corner
                ctx.arcTo(x + w, y, x + w, y + rectRadius, rectRadius);
                ctx.lineTo(x+w,y+h-rectRadius);    // right side
                // draw bottom right corner
                ctx.arcTo(x+w,y+h,x+w-rectRadius,y+h,rectRadius);

                ctx.lineTo(x+rectRadius,y+h);              // bottom side
                // draw bottom left corner
                ctx.arcTo(x,y+h,x,y+h-rectRadius,rectRadius);
                ctx.lineTo(x,y+rectRadius);                 // left side
                // draw top left corner
                ctx.arcTo(x,y,x+rectRadius,y,rectRadius);
            } else if (cornerDirection == "left") {
                var x = blurWidth + cornerHeight
                var y = blurWidth
                var w = rectWidth - 2 * blurWidth - cornerHeight
                var h = rectHeight - 2 * blurWidth

                ctx.moveTo(x + rectRadius, y);                 // top side
                ctx.lineTo(x + w - rectRadius, y);

                // draw top right corner
                ctx.arcTo(x + w, y, x + w, y + rectRadius, rectRadius);
                ctx.lineTo(x+w,y+h-rectRadius);    // right side
                // draw bottom right corner
                ctx.arcTo(x+w,y+h,x+w-rectRadius,y+h,rectRadius);

                ctx.lineTo(x+rectRadius,y+h);              // bottom side
                // draw bottom left corner
                ctx.arcTo(x,y+h,x,y+h-rectRadius,rectRadius);

                if (cornerPos < y + rectRadius + cornerWidth / 2) {
                    cornerPos = y + rectRadius + cornerWidth / 2
                }
                if (cornerPos > y + h - rectRadius - cornerWidth / 2) {
                    cornerPos = y + h - rectRadius - cornerWidth / 2
                }
                ctx.lineTo(x, cornerPos + cornerWidth / 2) /* corner */
                ctx.lineTo(x - cornerHeight, cornerPos)
                ctx.lineTo(x, cornerPos - cornerWidth / 2)

                ctx.lineTo(x,y+rectRadius);                 // left side
                // draw top left corner
                ctx.arcTo(x,y,x+rectRadius,y,rectRadius);
            } else if (cornerDirection == "right") {
                var x = blurWidth
                var y = blurWidth
                var w = rectWidth - 2 * blurWidth - cornerHeight
                var h = rectHeight - 2 * blurWidth

                ctx.moveTo(x + rectRadius, y);                 // top side
                ctx.lineTo(x + w - rectRadius, y);

                // draw top right corner
                ctx.arcTo(x + w, y, x + w, y + rectRadius, rectRadius);

                if (cornerPos < y + rectRadius + cornerWidth / 2) {
                    cornerPos = y + rectRadius + cornerWidth / 2
                }
                if (cornerPos > y + h - rectRadius - cornerWidth / 2) {
                    cornerPos = y + h - rectRadius - cornerWidth / 2
                }
                ctx.lineTo(x + w, cornerPos - cornerWidth / 2) /* corner */
                ctx.lineTo(x + w + cornerHeight, cornerPos)
                ctx.lineTo(x + w, cornerPos + cornerWidth / 2)

                ctx.lineTo(x+w,y+h-rectRadius);    // right side
                // draw bottom right corner
                ctx.arcTo(x+w,y+h,x+w-rectRadius,y+h,rectRadius);

                ctx.lineTo(x+rectRadius,y+h);              // bottom side
                // draw bottom left corner
                ctx.arcTo(x,y+h,x,y+h-rectRadius,rectRadius);

                ctx.lineTo(x,y+rectRadius);                 // left side
                // draw top left corner
                ctx.arcTo(x,y,x+rectRadius,y,rectRadius);
            }

            ctx.closePath();

            ctx.lineWidth = borderWidth
            ctx.strokeStyle = borderColor
            ctx.stroke()

            var gradient = ctx.createLinearGradient(rectWidth / 2, 0, rectWidth / 2, rectHeight);
            gradient.addColorStop(0.0, Qt.rgba(0, 0, 0, 0.55));
            gradient.addColorStop(1.0, Qt.rgba(0, 0, 0, 0.65));
            ctx.fillStyle = gradient
            ctx.fill()

            ctx.restore()
        }
    }

    Glow {
        anchors.fill: canvas
        visible: rect.withBlur
        radius: blurRadius
        samples: 16
        color: rect.blurColor
        source: canvas
    }
}
