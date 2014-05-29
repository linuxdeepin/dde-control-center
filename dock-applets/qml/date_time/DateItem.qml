import QtQuick 2.0
import Deepin.Widgets 1.0

Item {
    id: dateItem
    width: itemWidth
    height: itemHeight

    property color borderColor: Qt.rgba(1, 1, 1, 0.2)

    property var curDate: new Date(dateValue)
    property var dateValueArray: dateValue.split("-")

    property var lunarDayInfo: dbusLunarCalendar.GetLunarInfoBySolar(parseInt(dateValueArray[0]), parseInt(dateValueArray[1]), parseInt(dateValueArray[2]))
    property var hasFestival: lunarDayInfo[1] && (lunarDayInfo[0][8] || lunarDayInfo[0][9])
    Component.onCompleted: {
        //print(JSON.stringify(lunarDayInfo[0]))
    }

    Item {
        id: content
        width: itemWidth - 1
        height: itemHeight - 1

        Rectangle {
            anchors.fill: parent
            visible: dateItem.GridView.view.currentIndex == index
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(5/255, 112/255, 252/255, 0.2) }
                GradientStop { position: 1.0; color: Qt.rgba(36/255, 131/255, 1, 0.7) }
            } 
        }

        DssH2 {
            id: inText
            anchors.top: parent.top
            anchors.topMargin: lunarDayLabel.visible ? 2 : (parent.height - height)/2
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
            text: dayText
            color: isGrey ? "#4f4f4f": dconstants.fgColor
        }

        DssH3 {
            id: lunarDayLabel
            anchors.top: inText.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            font.pixelSize: 10
            color: isGrey ? "#4f4f4f": dconstants.fgColor
            text: {
                var info = lunarDayInfo[0]
                if(info[7]){
                    return info[7]
                }
                else if(info[4] == "初一"){
                    return info[3]
                }
                else{
                    return info[4]
                }
            }
            visible: false

            Component.onCompleted: {
                if(dssLocale.lang == "zh_CN"){
                    lunarDayLabel.visible = true
                }
            }
        }

    }

    Rectangle {
        id: rightBorder
        anchors.left: content.right
        width: 1
        height: parent.height
        color: borderColor
        visible: (index + 1) % 7 != 0
    }

    Rectangle {
        id: bottomBorder
        anchors.top: content.bottom
        width: parent.width
        height: 1
        color: borderColor
    }
}
