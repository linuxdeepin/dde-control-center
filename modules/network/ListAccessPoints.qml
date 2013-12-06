import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

Column {
    property variant accessPoints
    Repeater {
        model: accessPoints
        DLabel {
            height: 30
            verticalAlignment: Text.AlignVCenter
            width:parent.width
            text: accessPoints[index][0]
            DSepratorHorizontal{}
        }
    }
}
