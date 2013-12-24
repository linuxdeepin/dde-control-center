import QtQuick 2.1

Item {
    id: root
    width: 310
    height: 200

    signal itemSelected (int idx, string itemId)
    signal selectItemPrivate (string id)

    function select (itemId) {
        selectItemPrivate(itemId)
    }

    Flow {
        id: flow
        spacing: 5
        anchors.fill: parent

        Repeater {
            model: ListModel {
                ListElement {
                    itemId: "hello_world"
                    itemText: "Hello World"
                    itemWidth: 150
                }
                ListElement {
                    itemId: "linux_deepin"
                    itemText: "Linux Deepin"
                    itemWidth: 150
                }
                ListElement {
                    itemId: "deepin_is_great"
                    itemText: "Deepin is great"
                    itemWidth: 300
                }
            }
            Item {
                id: delegate
                state: "normal"
                width: Math.max(itemWidth, label.implicitWidth + 6)
                height: label.implicitHeight + 6

                function select () {
                    delegate.state = "selected"
                    root.itemSelected(index, itemId)
                }

                function deselect () {
                    delegate.state = "normal"
                }

                Connections {
                    target: root
                    onSelectItemPrivate: {
                        if (itemId == id) {
                            delegate.select()
                        } else {
                            delegate.deselect()
                        }
                    }
                }

                states: [
                    State {
                        name: "normal"
                        PropertyChanges {
                            target: box
                            color: "transparent"
                        }
                        PropertyChanges {
                            target: label
                            color: "white"
                        }
                    },
                    State {
                        name: "selected"
                        PropertyChanges {
                            target: box
                            color: "#0D0D0D"
                        }
                        PropertyChanges {
                            target: label
                            color: "#009EFF"
                        }
                    }
                ]

                Rectangle {
                    id: box
                    radius: 3

                    Text {
                        id: label
                        text: itemText
                        font.pixelSize: 12

                        anchors.centerIn: parent
                    }

                    anchors.fill: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.select(itemId)
                    }
                }
            }
        }
    }
}