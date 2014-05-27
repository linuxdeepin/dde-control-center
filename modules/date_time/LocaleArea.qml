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
            maximumFlickVelocity: 1000
            delegate: SelectItem {
                totalItemNumber: ListView.view.count
                selectItemId: localeExpand.currentLocale

                onSelectAction: {
                    gDate.SetLocale(itemId)
                }
            }

            Component.onCompleted: {
                var toSortResult = new Array()
                for(var i in localeExpand.localeToLanguage){
                    var tmp = new Array()
                    tmp.push(i)
                    tmp.push(localeExpand.localeToLanguage[i] + " (" + i + ")")
                    toSortResult.push(tmp)
                }
                toSortResult = windowView.sortSearchResult(toSortResult)

                for(var i in toSortResult){
                    model.append({
                        "item_id": toSortResult[i][0],
                        "item_name": toSortResult[i][1]
                    })
                }
            }
            DScrollBar {
                flickable: localeListView
            }
        }
    }
}

