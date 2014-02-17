import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Grub2 1.0

Rectangle {
    id: root
    color: "#1A1B1B"
    width: 310
    height: 600

    Grub2 { id: dbus_grub2 }
    Theme { id: dbus_grub2_theme }

    DssTitle {
        id: title
        z: 1
        text: "Grub2"
    }

    Flickable {
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: contentColumn.width
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            width: root.width
            height: preview.height + default_entry_expand.height + 
            delay_expand.height + normal_item_expand.height + selected_item_expand.height

            DSeparatorHorizontal{}
            Preview { 
                id: preview
                anchors.horizontalCenter: parent.horizontalCenter
            }
            DSeparatorHorizontal{}

            DBaseExpand {
                id: default_entry_expand

                header.sourceComponent: DDownArrowHeader {
                    text: "Default Entry"
                    onClicked: {
                        default_entry_expand.expanded = !default_entry_expand.expanded
                    }
                }
                content.sourceComponent: DMultipleSelectView {
                    id: default_entry_view
                    rows: dbus_grub2.GetSimpleEntryTitles().length
                    columns: 1

                    width: parent.width
                    height: rows * 30
                    singleSelectionMode: true

                    model: ListModel {}

                    Component.onCompleted: {
                        var entries = dbus_grub2.GetSimpleEntryTitles();
                        for (var i = 0; i < entries.length; i++) {
                            model.append({"label": entries[i], "selected": false})
                        }
                    }
                }
            }
            DSeparatorHorizontal{}
            DBaseExpand {
                id: delay_expand
                expanded: true

                header.sourceComponent: DBaseLine {
                    leftLoader.sourceComponent: DssH2 {
                        text: "Delay"
                    }
                }
                content.sourceComponent: DMultipleSelectView {
                    id: delay_view
                    rows: 1
                    columns: 7

                    width: parent.width
                    height: rows * 30
                    singleSelectionMode: true

                    model: ListModel {
                        ListElement {
                            label: "1s"
                            selected: false
                        }
                        ListElement {
                            label: "5s"
                            selected: false
                        }
                        ListElement {
                            label: "10s"
                            selected: false
                        }
                        ListElement {
                            label: "15s"
                            selected: false
                        }
                        ListElement {
                            label: "20s"
                            selected: false
                        }
                        ListElement {
                            label: "25s"
                            selected: false
                        }
                        ListElement {
                            label: "30s"
                            selected: false
                        }
                    }

                    onSelect: {
                    }
                }
            }

            DSeparatorHorizontal{}

            DBaseExpand {
                id: normal_item_expand

                header.sourceComponent: DDownArrowHeader {
                    text: "Item Color"
                    onClicked: {
                        normal_item_expand.expanded = !normal_item_expand.expanded
                    }
                }
                content.sourceComponent: ColorPicker{
                    width: root.width
                    height: 200
                }
            }

            DSeparatorHorizontal{}

            DBaseExpand {
                id: selected_item_expand

                header.sourceComponent: DDownArrowHeader {
                    text: "Selected Item Color"
                    onClicked: {
                        selected_item_expand.expanded = !selected_item_expand.expanded
                    }
                }
                content.sourceComponent: ColorPicker{
                    width: root.width
                    height: 200
                }
            }

            DSeparatorHorizontal{}
        }
    }
}