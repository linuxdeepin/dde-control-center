import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0

Item {
    anchors.fill: parent

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Display")
        }

        DSeparatorHorizontal {}

    }
}
