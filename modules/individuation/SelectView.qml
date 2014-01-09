import QtQuick 2.1

Item {
    id: root
    width: 100
    height: 100

    property int spacing: 5
    property int lineHeight: 22
    property int horizontalPadding: 6
    property variant model: ListModel{}

    signal itemSelected (int idx, string itemId)
    signal selectItemPrivate (string id)

    function select (itemId) {
        selectItemPrivate(itemId)
    }

    Flow {
        id: flow
        spacing: root.spacing
        anchors.fill: parent

        property alias horizontalPadding: root.horizontalPadding

        Repeater {
            model: root.model
            Item {

                id: delegate
                state: "normal"
                width: Math.max(label.implicitWidth + horizontalPadding, itemWidth)
                height: root.lineHeight

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

            function getStringWidth(str) {
                var tester = Qt.createQmlObject('import QtQuick 2.1; Text{visible:false}', root, "")
                tester.text = str
                return tester.width
            }

            /* Insert one item to the row which is from `rowStart'
               It returns true if the action success, otherwise false.
               */
            function insertToRow(rowStart, itemIndex) {
				var spacing = flow.spacing
                var totalWidth = 0
                for (var i = rowStart; i <= itemIndex; i++) {
                    totalWidth += getStringWidth(model.get(i).itemText) + horizontalPadding + spacing
                }
                if (totalWidth - spacing > root.width) {
                    return false
                } else {
                    return true
                }
            }

            function updateRow(rowStart, rowEnd) {
				var spacing = flow.spacing
                var totalWidth = 0
                var space = 0
                for (var i = rowStart; i <= rowEnd; i++) {
                    totalWidth += getStringWidth(model.get(i).itemText) + horizontalPadding + spacing 
                }
                space = root.width - (totalWidth - spacing)
                for (var i = rowStart; i <= rowEnd; i++) {
                    model.setProperty(i, "itemWidth", getStringWidth(model.get(i).itemText) + horizontalPadding + space / (rowEnd + 1 - rowStart))
                }
            }

            Component.onCompleted: {
                var spacing = flow.spacing
                var rowStart = 0
                var rowCount = 1

                for (var i = 0; i < model.count; i++) {
                    if (insertToRow (rowStart, i)) {
                        updateRow (rowStart, i)
                    } else {
                        rowStart = i
                        rowCount += 1
                        insertToRow (rowStart, i)
                        updateRow (rowStart, i)
                    }
                }
                root.height = root.lineHeight * rowCount
            }
        }
    }
}
