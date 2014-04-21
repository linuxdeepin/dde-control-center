import QtQuick 2.0
import QtQuick.Window 2.1

Loader {
    property url qmlPath: ""

    function show(){
        source = qmlPath
    }

    function hide(){
        source = ""
    }

    Component.onCompleted: {
        show()
    }
}
