import QtQuick 2.1
import QtQuick.Controls 1.0

Item {

    Rectangle {
        color: "#252627"
        anchors.fill: parent
        Input {
            id: searchKeywords
            width: 180
            anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20 }
        }

        Button {
            id: searchButton
            text: "search"
            height: 25
            width: 50
            anchors { top: parent.top; topMargin: 20; left: searchKeywords.right; leftMargin: 10;}
            onClicked: {
                console.log(searchKeywords.text);
                testDbus(searchKeywords.text);
            }
        }
    }
}
