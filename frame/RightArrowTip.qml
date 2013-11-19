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
    property int cornerWidth: 12
    property int cornerHeight: 8
	
    Canvas {
        id: canvas
        width: rectWidth
        height: rectHeight
        /* antialiasing: true */

        onPaint: {
            var ctx = getContext("2d")
            var x = blurWidth
            var y = blurWidth
            var w = rectWidth - 2 * blurWidth - cornerHeight
            var h = rectHeight - 2 * blurWidth
            var cornerPos = h/2

            ctx.save()
            ctx.clearRect(0, 0, canvas.width, canvas.height)

            ctx.beginPath();
            ctx.moveTo(x + rectRadius, y);                 // top side
            ctx.lineTo(x + w - rectRadius, y);
            // draw top right corner
            ctx.arcTo(x + w, y, x + w, y + rectRadius, rectRadius);

            ctx.lineTo(x + w, y + cornerPos - cornerWidth/2)
            ctx.lineTo(x + w + cornerHeight, y + cornerPos)
            ctx.lineTo(x + w, y + cornerPos + cornerWidth/2)
            ctx.lineTo(x+w,y+h - rectRadius);

            // draw bottom right corner
            ctx.arcTo(x+w,y+h,x+w-rectRadius,y+h,rectRadius);

            ctx.lineTo(x + rectRadius, y+h);    // bottom side

            // draw bottom left corner
            ctx.arcTo(x,y+h,x,y+h-rectRadius,rectRadius);
            ctx.lineTo(x,y+rectRadius);                 // left side
            // draw top left corner
            ctx.arcTo(x,y,x+rectRadius,y,rectRadius);
            ctx.closePath();

            ctx.lineWidth = borderWidth
            ctx.strokeStyle = borderColor
            ctx.stroke()
            ctx.fillStyle = fillColor
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
