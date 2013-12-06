import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

DBaseHeader {
    id: header

    property string text: "untitled"
    property var dconstants: DConstants {}

    signal toggled

    title: Label {
        id: titleLabel
        text: header.text
        color: dconstants.fgColor
	}

    button: Image {
        id: arrow
        source: "images/arrow_right_normal.png"
        MouseArea {
            anchors.fill:parent
            onClicked: {
                toggled()
            }
        }
    }
}
