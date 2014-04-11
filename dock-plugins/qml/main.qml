import QtQuick 2.0
import QtQuick.Window 2.1

QtObject {
    id: root

    property var dssLoader: Loader{
        source: "dss/DssWindow.qml"
    }

    property var powerLoader: Loader{
        source: "power/PowerWindow.qml"
    }
}
