import QtQuick 2.1
import Deepin.Widgets 1.0
import QtQuick.Controls 1.0
import "../shared/"

Item {
    id: remotingItem

    property var stackViewPages: {
        "mainPage": Qt.resolvedUrl("MainPanel.qml"),
        "accessPage": Qt.resolvedUrl("AccessPanel.qml"),
        "sharePage": Qt.resolvedUrl("SharePanel.qml")
    }

    function changePage(page) {
        stackView.push({
            "item": stackViewPages[page],
            "properties": { "width": parent.width }
        })
        stackView.currentItemId = page
    }


    function reset(){
        stackView.reset()
    }

    StackView {
        id:stackView
        width: parent.width
        height: parent.height

        property string currentItemId: ""

        function reset(){
            stackView.pop(null)
            stackView.currentItemId = "mainPage"
        }

        Component.onCompleted: {
            stackView.push({
                "item": stackViewPages["mainPage"],
                "properties": { "width": parent.width }
            })
            stackView.currentItemId = "mainPage"
        }
    }
}
