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

    function timeoutToIndex(timeout) {
        switch (timeout) {
            case 1: return 0; break
            case 5: return 1; break
            case 10: return 2; break
            case 15: return 3; break
            case 20: return 4; break
            case 25: return 5; break
            case 30: return 6; break
        }
    }

    function indexToTimeout(idx) {
        switch (idx) {
            case 0: return 1; break
            case 1: return 5; break
            case 2: return 10; break
            case 3: return 15; break
            case 4: return 20; break
            case 5: return 25; break
            case 6: return 30
        }
    }

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
            height: preview.height + default_entry_expand.height + delay_expand.height
            + normal_item_expand.height + selected_item_expand.height + 10
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
                            model.append({"label": entries[i], "selected": entries[i] == dbus_grub2.defaultEntry})
                        }
                    }

                    onSelect: {
                        print("select..." + index)
                        dbus_grub2.defaultEntry = dbus_grub2.GetSimpleEntryTitles()[index]
                    }

                    Connections {
                        target: dbus_grub2
                        onDefaultEntryChanged: {
                            print("default entry changed")
                            var i = dbus_grub2.GetSimpleEntryTitles().indexOf(dbus_grub2.defaultEntry)
                            if (i != -1) {
                                default_entry_view.selectItem(i)
                            }
                        }
                    }
                }
            }
            DSeparatorHorizontal{}
            DBaseExpand {
                id: delay_expand
                header.sourceComponent: DDownArrowHeader {
                    text: "Delay"
                    onClicked: {
                        delay_expand.expanded = !delay_expand.expanded
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

                    Component.onCompleted: {
                        delay_view.selectItem(root.timeoutToIndex(dbus_grub2.timeout))
                    }

                    onSelect: {
                        print("select..." + index)
                        dbus_grub2.timeout = root.indexToTimeout(index)
                    }

                    Connections {
                        target: dbus_grub2
                        onTimeoutChanged: {
                            print("timeout changed")
                            delay_view.selectItem(root.timeoutToIndex(dbus_grub2.timeout))
                        }
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
                    id: normal_picker
                    width: root.width
                    height: 180

                    Component.onCompleted: {
                        selectColor(dbus_grub2_theme.itemColor)
                    }

                    onColorSet: {
                        dbus_grub2_theme.itemColor = clr + ""
                    }

                    Connections {
                        target: dbus_grub2_theme
                        onItemColorChanged: {
                            print("changed..." + dbus_grub2_theme.itemColor)
                            normal_picker.selectColor(dbus_grub2_theme.itemColor)
                        }
                    }
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
                    id: selected_picker
                    width: root.width
                    height: 180

                    Component.onCompleted: {
                        selectColor(dbus_grub2_theme.selectedItemColor)
                    }

                    onColorSet: {
                        dbus_grub2_theme.selectedItemColor = clr + ""
                    }

                    Connections {
                        target: dbus_grub2_theme
                        onItemColorChanged: {
                            print("changed..." + dbus_grub2_theme.selectedItemColor)                            
                            selected_picker.selectColor(dbus_grub2_theme.selectedItemColor)
                        }
                    }
                }
            }

            DSeparatorHorizontal{}
        }
    }
}