/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Timedate 1.0
import DBus.Com.Deepin.Api.LunarCalendar 1.0
import DBus.Com.Deepin.Daemon.Search 1.0
import "../shared"
import "calendar_core.js" as CalendarCore

DFlickable {
    id: dateTimeModule
    anchors.fill: parent

    contentWidth: width
    contentHeight: height < 768 ? 768 : height

    FontLoader { id: fixedFont; source: "MavenProLight-200.otf" }
    property var gDate: Timedate {
        onTimezoneChanged: {
            timezoneChangedCallbackTimer.restart()
        }
    }
    property var currentTimezone: gDate.timezone
    property var dstList
    property var dconstants: DConstants {}
    property bool showedTimezoneList: false
    property string lang: 'en_US'
    property var locale: Qt.locale()

    property var dbusLunarCalendar: LunarCalendar {}

    property var dbusSearch: Search {}

    property var globalDate: new Date()

    function reduceTextLengthWithFlag(preText,fontPixelSize,maxWidth,splitFlag){
        if (Math.ceil(getStringPixelSize(preText,fontPixelSize)) < maxWidth){
            return preText
        }

        var tmpTextArray = preText.split(splitFlag)
        for (var i = 1; i < tmpTextArray.length; i ++){
            var tmpString = tmpTextArray.slice(0,-i).join(splitFlag)
            var width = Math.ceil(getStringPixelSize(tmpString,fontPixelSize))
            if (width <= maxWidth){
                return tmpString
            }
        }
    }

    Component.onCompleted: {
        lang = dsslocale.lang
    }

    Timer{
        id: timezoneChangedCallbackTimer
        interval: 800
        onTriggered: {
            Date.timeZoneUpdated()
            globalDate = new Date()
            calendarObj.currentSelectedDateValue = calendarObj.currentDateValue
        }
    }

    Timer {
        running: !timezoneChangedCallbackTimer.running
        repeat: true
        interval: 500
        onTriggered: {
            dateTimeModule.globalDate= new Date()
        }
    }

    DssTitle {
        id:datetimeDT
        text: modulesId.moduleLocaleNames["date_time"]
    }

    DSeparatorHorizontal {
        anchors.top: datetimeDT.bottom
    }

    ListView {
        anchors.top:datetimeDT.bottom
        height: parent.height - datetimeDT.height
        width: parent.width
        model: itemModel
        clip: true
    }

    VisualItemModel
    {
        id: itemModel

        DSeparatorHorizontal{}

        TimeBox {
            id: timeBox
            use24Hour: twentyFourHourSetBox.active
            editable: !gDate.nTP
        }

        DSeparatorHorizontal {}

        DSwitchButtonHeader {
            id: autoSetTimeBox
            text: dsTr("Sync Automatically")
            width: parent.width
            height: showedTimezoneList ? 0 : 30
            visible: !showedTimezoneList
            active: gDate.nTP

            onClicked: {
                gDate.SetNTP(active)
                if(active && timeBox.inEdit){
                    timeBox.showTimeNormal()
                }
            }
        }

        DSeparatorHorizontal {visible: !showedTimezoneList}

        DSwitchButtonHeader {
            id: twentyFourHourSetBox
            text: dsTr("Use 24-hour clock")
            active: gDate.use24HourFormat
            height: showedTimezoneList ? 0 : 30
            visible: !showedTimezoneList

            onClicked: {
                gDate.use24HourFormat = active
            }
        }

        DSeparatorHorizontal {visible: !showedTimezoneList}

        TimezoneArea {
            id: timeZoneArea
            onIsAddState: showedTimezoneList = state
            listAreaMaxHeight: isInAddState ? dateTimeModule.contentHeight - timeBox.height - datetimeDT.height: 230
        }

        DBaseExpand {
            id: dstRec
            expanded: true
            opacity: 0
            height: opacity > 0 ? 60 : 0
            visible: !showedTimezoneList

            property var pDSTList

            DSeparatorHorizontal {anchors.top: parent.top}

            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: dsTr("Adjust DST clock ")
                }
            }
            content.sourceComponent: GridView{
                id: dstListView
                width: parent.width
                height: 30

                cellWidth: width/3
                cellHeight: 24

                Connections {
                    target: timeZoneArea
                    onHasDSTList:{
                        if (sHasDST){
                            dstRec.opacity = 1
                            dstListView.fillModel(sDSTList)
                        }
                        else
                            dstRec.opacity = 0
                    }
                }

                function getLabelName(value){
                    if (value == 0)
                        return dsTr("None")
                    else{
                        if (value > 0)
                            return "+" + value.toString() + "h"
                        else
                            return value.toString() + "h"
                    }
                }

                function fillModel(pDSTList){
                    dstListView.model.clear()

                    for (var key in pDSTList){
                        var value = key / 3600
                        dstListView.model.append({
                                         "item_label": getLabelName(value),
                                         "item_value": key.toString(),
                                         "item_zone": pDSTList[key]
                                     })
                    }
                }

                model:ListModel {}

                delegate: DSTItem {
                    currentValue: typeof(gDate.dSTOffset) == "undefined" ? 0 : gDate.dSTOffset
                    onSelectAction: {
                        print ("==> Set DST:",itemValue,itemZone,typeof(gDate.dSTOffset),gDate.dSTOffset)
                        gDate.SetTimezone(timezone)
                        gDate.dSTOffset = itemValue
                    }
                }
            }

            Behavior on height {
                PropertyAnimation {target: dstRec; property: "height"; duration: 150}
            }
            Behavior on opacity {
                NumberAnimation {target: dstRec; property: "opacity"; duration: 150}
            }
        }


        DBaseLine {
            id: dateBoxTitle
            width: parent.width
            height: showedTimezoneList ? 0 : 30
            visible: !showedTimezoneList
            rightMargin: 10

            leftLoader.sourceComponent: DLabel {
                font.pixelSize: 13
                text: dsTr("Date")
            }

            rightLoader.sourceComponent: Item {
                height: dateBoxTitle.height

                property bool normalState: true


                ItemTooltip {
                    id:dateTimeTooltip
                    anchors.right: setImage.left
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    width: 200
                    delayShowInterval: 500
                    textItem.font.pixelSize: 12
                }

                DImageButton {
                    id: setImage
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 7
                    normal_image: "images/setdate_normal.png"
                    hover_image: "images/setdate_hover.png"
                    press_image: "images/setdate_press.png"
                    visible: parent.normalState && !gDate.nTP
                    onClicked: {
                        parent.normalState = false
                        dateTimeTooltip.hideTipImmediately()
                    }
                    onEntered: {
                        dateTimeTooltip.tooltip = dsTr("Set Date")
                        dateTimeTooltip.showTip()
                    }
                    onExited: {
                        dateTimeTooltip.hideTip()
                    }
                }

                DTextButton {
                    id: cancelButton
                    text: dsTr("Cancel")
                    visible: opacity != 0
                    opacity: parent.normalState ? 0 : 1
                    anchors.right: confirmButton.left
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter

                    Behavior on opacity {
                        PropertyAnimation { duration: 200 }
                    }

                    onClicked: {
                        parent.normalState = true
                    }
                }

                DTextButton {
                    id:confirmButton
                    text: dsTr("Confirm")
                    visible: opacity != 0
                    opacity: parent.normalState ? 0 : 1
                    anchors.right: parent.right
                    anchors.rightMargin: 7
                    anchors.verticalCenter: parent.verticalCenter

                    Behavior on opacity {
                        PropertyAnimation { duration: 200 }
                    }

                    onClicked: {
                        var dateString = calendarObj.currentSelectedDateValue
                        if (dateString == "")
                            return
                        var dateValue = dateString.split("-")

                        var year = dateValue[0]
                        var month = dateValue[1]
                        var date = dateValue[2]
                        var hour = globalDate.toLocaleTimeString(locale,"HH")
                        var min = globalDate.toLocaleTimeString(locale,"mm")
                        var second = globalDate.toLocaleTimeString(locale,"ss")

                        gDate.SetDate(year,month,date,hour,min,second,0)
                        parent.normalState = true
                    }
                }
            }
        }

        DSeparatorHorizontal {visible: !showedTimezoneList}

        Calendar {
            height: showedTimezoneList ? 0 : 333
            visible: !showedTimezoneList
            id: calendarObj
        }
    }
}
