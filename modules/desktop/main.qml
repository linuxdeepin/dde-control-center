import QtQuick 2.1

Rectangle {
    id: module
    width: 300
    height: 600
    color: "#252627"

    property color labelFontColor: "white"

    property int comboLeftLabelWidth: 100
    property int comboWidth: 200

    TitleSeparator {
        id: title_sep_icons

        fontColor: labelFontColor
        titleIcon: "images/icon.png"
        titleLabel: qsTr("Desktop Icons")

        anchors.top: parent.top
    }

    Column {
        id: desktop_icons
        anchors.top: title_sep_icons.bottom
        anchors.left: parent.left
        anchors.leftMargin: 20

        CheckBox {
            text: qsTr("Computer")
            fontColor: labelFontColor
        }
        CheckBox {
            text: qsTr("Home")
            fontColor: labelFontColor
        }
        CheckBox {
            text: qsTr("Trash")
            fontColor: labelFontColor
        }
        CheckBox {
            text: qsTr("Software Center")
            fontColor: labelFontColor
        }
    }

    TitleSeparator {
        id: title_sep_dock

        fontColor: labelFontColor
        titleIcon: "images/dock.png"
        titleLabel: qsTr("Dock")

        anchors.top: desktop_icons.bottom
    }

    Row {
        id: row_display_style
        anchors.top: title_sep_dock.bottom

        Column {
            width: comboLeftLabelWidth
            Text {
                text: qsTr("Display Style")
                color: labelFontColor
            }
        }
        Column {
            width: comboWidth
            ComboBox {
                id: display_combo
                width: 200

                model: ListModel {
                    ListElement { text: "Banana"; color: "Yellow" }
                    ListElement { text: "Apple"; color: "Green" }
                    ListElement { text: "Coconut"; color: "Brown" }
                }
            }
        }
    }

    TitleSeparator {
        id: title_sep_hotspot

        fontColor: labelFontColor
        titleIcon: "images/hot.png"
        titleLabel: qsTr("HotSpot")

        anchors.top: row_display_style.bottom
    }

    Row {
        id: row_hotspot_top_left
        anchors.top: title_sep_hotspot.bottom

        Column {
            width: comboLeftLabelWidth
            Text {
                text: qsTr("Top Left")
                color: labelFontColor
            }
        }
        Column {
            width: comboWidth
            ComboBox {
                id: hotspot_top_left_combo
                width: 200

                model: ListModel {
                    ListElement { text: "Banana"; color: "Yellow" }
                    ListElement { text: "Apple"; color: "Green" }
                    ListElement { text: "Coconut"; color: "Brown" }
                }
            }
        }
    }
    Row {
        id: hotspot_bottom_right
        anchors.top: row_hotspot_top_left.bottom

        Column {
            width: comboLeftLabelWidth
            Text {
                text: qsTr("Bottom Right")
                color: labelFontColor
            }
        }
        Column {
            width: comboWidth
            ComboBox {
                id: bottom_right_combo
                width: 200

                model: ListModel {
                    ListElement { text: "Banana"; color: "Yellow" }
                    ListElement { text: "Apple"; color: "Green" }
                    ListElement { text: "Coconut"; color: "Brown" }
                }
            }
        }
    }
}
