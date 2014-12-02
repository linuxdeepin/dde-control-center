import QtQuick 2.2
import QtGraphicalEffects 1.0
import "../shared"

Item {
    id: warning_arrow_rect
    width: 400
    height: warning_arrow_rect_text.implicitHeight + (textVertPadding + shadowWidth) * 2 + arrowHeight * 2

    property int shadowWidth: 5
    property alias arrowHeight: rect.arrowHeight
    property alias warningText: warning_arrow_rect_text.text

    property int textHoriPadding: 5
    property int textVertPadding: 5

    Item {
        id: bg
        anchors.fill: parent

        ArrowRect {
            id: rect
            x: warning_arrow_rect.shadowWidth
            y: warning_arrow_rect.shadowWidth
            width: parent.width - 2 * warning_arrow_rect.shadowWidth
            height: parent.height - 2 * warning_arrow_rect.shadowWidth
            lineWidth: 1
            fillStyle: Qt.rgba(0, 0, 0, 0.7)
            strokeStyle: Qt.rgba(1, 1, 1, 0.2)
            arrowPosition: 0.1
        }
    }

    Glow {
        anchors.fill: bg
        radius: parent.shadowWidth
        samples: 16
        color: "black"
        source: bg
    }

    Text {
        id: warning_arrow_rect_text
        color: "#ff8c03"
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: arrowHeight * 3 + 2

        x: warning_arrow_rect.shadowWidth + warning_arrow_rect.textHoriPadding
        y: warning_arrow_rect.shadowWidth + warning_arrow_rect.arrowHeight + warning_arrow_rect.textVertPadding
        width: parent.width - 2 * (warning_arrow_rect.textHoriPadding + warning_arrow_rect.shadowWidth)
        wrapMode: Text.WordWrap
    }
}
