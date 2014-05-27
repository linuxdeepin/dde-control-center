import QtQuick 2.2
import Deepin.Widgets 1.0
import "../shared"

DBaseExpand {
    id: localeExpand
    property string currentLocale: gDate.currentLocale
    property var localeToLanguage: gDate.localeListMap

    property int listAreaMaxHeight: 100

    header.sourceComponent: DDownArrowHeader {
        text: dsTr("Language")
        hintText: " (" + localeExpand.localeToLanguage[localeExpand.currentLocale]+ ")"
        width: 310
        anchors.leftMargin: 2
        anchors.rightMargin: 10
        
        onClicked: {
            localeExpand.expanded = !localeExpand.expanded
        }
    }

    content.sourceComponent: Rectangle {
        width: parent.width
        height: localeExpand.listAreaMaxHeight
        color: dconstants.contentBgColor

        ListView {
            id: localeListView
            width: parent.width
            height: parent.height
            model: ListModel {}
            delegate: SelectItem {
                totalItemNumber: ListView.view.count
                selectItemId: localeExpand.currentLocale

                onSelectAction: {
                    gDate.SetLocale(itemId)
                }
            }

            Component.onCompleted: {
                for(var i in localeExpand.localeToLanguage){
                    model.append({
                        "item_id": i,
                        "item_name": localeExpand.localeToLanguage[i] + " (" + i + ")"
                    })
                }
            }
            DScrollBar {
                flickable: localeListView
            }
        }
    }
}

