import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

ToggleSwitcher {
    toggled: true

    id: c1
    title: dsTr("Wired Connections")

    content: Column {
        Text { text:"4444444"}
        Text { text:"4444444"}
        Text { text:"4444444"}
        Text { text:"4444444"}
        DSeparatorHorizontal {}
    }
}
