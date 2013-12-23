import QtQuick 2.0

Rectangle {
    color: "lightsteelblue"; width: 300; height: 200

    ListView {
        anchors.fill: parent
        focus: true

        model: ListModel {
            ListElement { name: "Bob" }
            ListElement { name: "John" }
            ListElement { name: "Michael" }
        }

        delegate: FocusScope {
            id: scope
            width: childrenRect.width; height: childrenRect.height
            x:childrenRect.x; y: childrenRect.y
            Rectangle {
                height: 30
                width: 250
                focus: true
                Keys.onReturnPressed: console.log(name)
                color: scope.ListView.isCurrentItem? "red" : "white"

                Text {
                    anchors.left: parent.left
                    text: "left"
                }

                Rectangle {
                    anchors.right: parent.right
                    width: 100
                    height: parent.height
                    focus: true

                    Text{
                        anchors.centerIn: parent
                        text: name
                        focus: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            print(index)
                            scope.ListView.view.currentIndex = index
                            print(parent.activeFocus)
                        }
                    }
                }

            }
        }
    }
}
