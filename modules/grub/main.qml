import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Grub2 1.0
import "../shared"

Rectangle {
    id: grub
    color: dconstants.bgColor
    anchors.fill: parent

    Grub2 { id: dbus_grub2 }
    Theme { id: dbus_grub2_theme }

    function reset() {
        dbus_grub2.Reset()
    }

    function indexToLabel(idx) {
        switch (idx) {
            case 0: return "0s"; break
            case 1: return "5s"; break
            case 2: return "10s"; break
            case 3: return "15s"; break
            case 4: return "20s"; break
            case 5: return "25s"; break
            case 6: return "30s"
        }
    }

    function timeoutToIndex(timeout) {
        switch (timeout) {
            case 0: return 0; break
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
            case 0: return 0; break
            case 1: return 5; break
            case 2: return 10; break
            case 3: return 15; break
            case 4: return 20; break
            case 5: return 25; break
            case 6: return 30
        }
    }

    Item {
        id: title_row
        z: 3
        width: parent.width
        height: title.height

        DssTitle {
            id: title
            text: dsTr("Startup Screen")
        }

        ResetButton {
            onClicked: reset()

            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Flickable {
        anchors.top: title_row.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: contentColumn.width
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            width: grub.width
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
                    text: dsTr("Default Entry")
                    onClicked: {
                        default_entry_expand.expanded = !default_entry_expand.expanded
                    }
                }
                content.sourceComponent: Rectangle {
                    width: parent.width
                    height: childrenRect.height
                    color: dconstants.contentBgColor

                    ListView {
                        id: entries_list
                        focus: true
                        width: parent.width
                        height: childrenRect.height
                        model: ListModel {}
                        delegate: SelectItem{
                            labelLeftMargin: 6
                            totalItemNumber: dbus_grub2.GetSimpleEntryTitles().length
                            selectItemId: dbus_grub2.defaultEntry

                            onSelectAction: {
                                dbus_grub2.defaultEntry = itemName
                            }
                        }
                        anchors.left: parent.left
                        anchors.leftMargin: 10

                        Component.onCompleted: {
                            var entries = dbus_grub2.GetSimpleEntryTitles();
                            for (var i = 0; i < entries.length; i++) {
                                model.append({"item_name": entries[i], "item_id": entries[i]})
                            }
                        }


                        Connections {
                            target: dbus_grub2
                            onDefaultEntryChanged: {
                                if (entries_list.model.get(entries_list.currentIndex).name != dbus_grub2.defaultEntry) {
                                    for (var i = 0; i < entries_list.count; i++) {
                                        if (dbus_grub2.defaultEntry == entries_list.model.get(i).name) {
                                            entries_list.currentIndex = i
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            DSeparatorHorizontal{}

            DBaseExpand {
                id: delay_expand
                header.sourceComponent: DDownArrowHeader {
                    text: dsTr("Delay")
                    onClicked: {
                        delay_expand.expanded = !delay_expand.expanded
                    }
                }
                content.sourceComponent: GridView {
                    id: delay_view
                    width: parent.width
                    height: 30

                    cellWidth: width/7
                    cellHeight: 30

                    model: {
                        var model = listModelComponent.createObject(delay_view, {})
                        for(var i=0; i<7; i++){
                            model.append({
                                             "item_label": indexToLabel(i),
                                             "item_value": indexToTimeout(i)
                                         })
                        }
                        return model
                    }

                    delegate: PropertyItem {
                        currentValue: dbus_grub2.timeout
                        onSelectAction: {
                            dbus_grub2.timeout = itemValue
                        }
                    }
                }
            }

            DSeparatorHorizontal{}

            DBaseExpand {
                id: normal_item_expand

                header.sourceComponent: DDownArrowHeader {
                    text: dsTr("Item Color")
                    onClicked: {
                        normal_item_expand.expanded = !normal_item_expand.expanded
                    }
                }
                content.sourceComponent: ColorPicker{
                    id: normal_picker
                    width: grub.width
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
                    text: dsTr("Selected Item Color")
                    onClicked: {
                        selected_item_expand.expanded = !selected_item_expand.expanded
                    }
                }
                content.sourceComponent: ColorPicker{
                    id: selected_picker
                    width: grub.width
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
