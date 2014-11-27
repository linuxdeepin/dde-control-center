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

import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared/"

Column {
    id: timezoneArea
    width: parent.width
    height: {
        if(currentActionStateName == "addButton"){
            return addTimezoneListArea.height + 34
        }
        else{
            return userTimezoneListArea.height + 34
        }
    }

    Behavior on height {
        PropertyAnimation { easing.type: Easing.InQuad; duration: 300 }
    }

    signal hasDSTList(bool sHasDST ,var sDSTList)
    signal isAddState(bool state)
    property bool isInAddState:false

    property var timezoneInformation: {
        var zoneList = new Array()
        var cityList = new Array()
        var codeList = new Array()
        var allZoneInfo = gDate.GetAllZoneSummary()
        for(var i in allZoneInfo){
            var info = allZoneInfo[i]
            zoneList.push(info[0])
            cityList.push(info[1])
            codeList.push(info[2])
        }
        return {"zone":zoneList,"city": cityList,"code": codeList}
    }

    property var zoneCodePair: {
        var zoneCodeArray = new Array()
        var codeArray = new Array()

        for(var i=0;i<timezoneInformation["code"].length;i++){
            var code = timezoneInformation["code"][i]

            if (codeArray.indexOf(code) >= 0)
                continue
            codeArray.push(code)

            var zoneArray = new Array()
            for (var j = 0; j < timezoneInformation["zone"].length; j ++){
                if (timezoneInformation["code"][j] == code){
                    zoneArray.push(timezoneInformation["zone"][j])
                }
            }
            zoneCodeArray.push({"zones": zoneArray,"code": code})
        }

        return zoneCodeArray
    }

    property var userTimezoneList: gDate.userTimezones

    property var currentTimezone: gDate.currentTimezone


    property string currentActionStateName: ""
    property var listModelComponent: DListModelComponent {}
    property string searchMd5: {
        var retList = dbusSearch.NewSearchWithStrList(timezoneInformation.city)
        return retList[0]
    }

    property int listAreaMaxHeight

    function isInUserTimezone(zone){
        for (var i = 0; i < userTimezoneList.length; i ++){
            if (getOffsetByZone(userTimezoneList[i]) == getOffsetByZone(zone))
                return true
        }
        return false
    }

    function getCityByZone(zone){
        if (zone == ""){
            return ""
        }
        else{
            if (!gDate.GetZoneInfo(zone))
                return ""
            return gDate.GetZoneInfo(zone)[0][1]
        }
    }

    function getDSTByZone(zone){
        var tmpOffset = getOffsetByZone(zone)
        var tmpArray
        for (var i = 0; i < zoneCodePair.length; i ++){
            if (zoneCodePair[i]["code"] == tmpOffset){
                tmpArray = zoneCodePair[i]["zones"]
                break
            }
        }
        var DSTArray = new Object()
        for (i = 0; i < tmpArray.length; i ++){
            if (gDate.GetZoneInfo(tmpArray[i])[1][0] == 0 || gDate.GetZoneInfo(tmpArray[i])[1][1] == 0){
                DSTArray[0] = tmpArray[i]
                continue
            }
            else{
                DSTArray[gDate.GetZoneInfo(tmpArray[i])[1][2]] = tmpArray[i]
            }
        }


        return DSTArray
    }

    function getOffsetByZone(zone){
        if (zone == "")
            return ""
        else{
            if (!gDate.GetZoneInfo(zone))
                return ""
            return gDate.GetZoneInfo(zone)[0][2]
        }
    }

    function getCityList(cityArray){
        if (typeof(cityArray) == "undefined")
            return ""
        else{
            var tmpList = ""
            for (var i = 0; i < cityArray.length - 1; i ++){
                if (!gDate.GetZoneInfo(cityArray[i]))
                    continue
                tmpList += gDate.GetZoneInfo(cityArray[i])[0][1] + ", "
            }

            if (gDate.GetZoneInfo(cityArray[cityArray.length - 1]))
                tmpList += gDate.GetZoneInfo(cityArray[cityArray.length - 1])[0][1]

            return tmpList
        }
    }

    function getCityListByOffset(offset){
        for (var i = 0; i < zoneCodePair.length; i ++){
            if (zoneCodePair[i]["code"] == offset){
                return getCityList(zoneCodePair[i]["zones"])
            }
        }
        return ""
    }

    function getOffsetName(offset){
        var result = offset / 3600
        var hour=parseInt(result);
        var min=result-hour;

        min = min < 0 ? (-min * 60).toString() : (min * 60).toString()
        min = min == "0" ? "00" : min
        if (hour >= 0){
            if (hour >= 10)
                hour = hour.toString()
            else
                hour = "0" + hour.toString()
        }
        else{
            if (hour > -10){
                hour = "-0" + (-hour).toString()
            }
            else
                hour = hour.toString()
        }

        if (offset >= 0)
            return "UTC+" + hour + ":" + min
        else
            return "UTC" + hour + ":" + min
    }

    function getZoneByOffset(offset){
        for (var i = 0; i < zoneCodePair.length; i ++){
            if (zoneCodePair[i]["code"] == offset)
                return zoneCodePair[i]["zones"][0]
        }
        return ""
    }

    onUserTimezoneListChanged: {
        if (timeZoneArea.currentActionStateName == "deleteButton")
            userTimezoneListView.fillModel(true)
        else{
            delayAddTimer.stop()
            delayAddTimer.start()
        }
    }

    onCurrentTimezoneChanged: userTimezoneListView.updateCurrentTimezone()
    onHeightChanged: gButtonToolTip.hideToolTip()
    Timer {
        id:delayAddTimer
        repeat: false
        interval: 300
        running: false
        onTriggered: {
//            print ("==> Fill user timezone model...")
            userTimezoneListView.fillModel(false)
        }
    }

    DBaseLine {
        id: titleLine
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Time zone")
        }

        rightLoader.sourceComponent: StateButtons {
            deleteButton.visible: userTimezoneList.length > 1
            onCurrentActionStateNameChanged: {
                // before changed
                if(timezoneArea.currentActionStateName == "addButton"){
                    for(var key in addTimezoneListView.selectedItemDict){
                        if(addTimezoneListView.selectedItemDict[key]){
                            gDate.AddUserTimezone(key)
                        }
                    }

                    timeZoneArea.isAddState(false)
                    timeZoneArea.isInAddState = false
                }
                else if (timezoneArea.currentActionStateName == "deleteButton"){
                    userTimezoneListView.updateDeleteActionFlag(false)
                }

                if (currentActionStateName == "deleteButton")
                    userTimezoneListView.updateDeleteActionFlag(true)
                // before changed

                timezoneArea.currentActionStateName = currentActionStateName

                // after changed
                if(timezoneArea.currentActionStateName == "addButton"){
                    addTimezoneListView.fillModel()

                    timeZoneArea.isAddState(true)
                    timeZoneArea.isInAddState = true
                }
                // after changed
            }
        }

    }

    DSeparatorHorizontal {}

    Rectangle {
        id: userTimezoneListArea
        width: parent.width
        height: childrenRect.height
        color: dconstants.contentBgColor
        visible: timezoneArea.currentActionStateName != "addButton"

        ListView {
            id: userTimezoneListView
            width: parent.width
            height: model.count * 50 > listAreaMaxHeight ? listAreaMaxHeight : model.count * 50
            clip: true

            function fillModel(ida){
                model.clear()

                var currentTimezoneOffset = getOffsetByZone(gDate.currentTimezone)

                for (var i = 0; i < userTimezoneList.length; i ++){
                    var tmpZone = userTimezoneList[i]
                    if (!gDate.GetZoneInfo(tmpZone)){
                        continue
                    }

                    var tmpTimezoneOffset = getOffsetByZone(tmpZone)
                    if (isInList(tmpTimezoneOffset))
                        continue

                    model.append({
                                     "timezone" : tmpZone,
                                     "cityList": getCityListByOffset(tmpTimezoneOffset),
                                     "offsetName": getOffsetName(tmpTimezoneOffset),
                                     "timezoneOffset":tmpTimezoneOffset,
                                     "DSTList": getDSTByZone(tmpZone),
                                     "isCurrentTimezone": tmpTimezoneOffset == currentTimezoneOffset,
                                     "inDeleteAction": ida
                                 })
                }
            }

            function updateDeleteActionFlag(flag){
                for (var i = 0; i < model.count; i ++){
                    model.get(i).inDeleteAction = flag
                }
            }

            function updateCurrentTimezone(){
                var currentTimezoneOffset = getOffsetByZone(gDate.currentTimezone)
                for (var i = 0; i < model.count; i ++){
                    if (model.get(i).timezoneOffset == currentTimezoneOffset)
                        model.get(i).isCurrentTimezone = true
                    else
                        model.get(i).isCurrentTimezone = false
                }
            }

            function isInList(offset){
                for (var i = 0 ; i < model.count; i ++){
                    if (model.get(i).timezoneOffset == offset)
                        return true
                }
                return false
            }

            Component.onCompleted: fillModel(false)

            model: ListModel {}

            delegate: UserTimezoneItem{
                onDeleteAction: {
                    print ("==> Deletting usertimezone...",timezone)
                    gDate.DeleteUserTimezone(timezone)
                }
                onSelectAction: {
                    timeZoneArea.hasDSTList(pHasDST,pDSTList)

                    if (pIsCurrentTimezone)
                        return
                    print ("==> Set usertimezone...",timezone)
                    gDate.SetTimezone(timezone)
                    gDate.dSTOffset = 0
                }
            }

            DScrollBar {
                flickable: userTimezoneListView
            }
        }
    }

    Rectangle{
        id: addTimezoneListArea
        width: parent.width
        height: listAreaMaxHeight
        color: dconstants.contentBgColor
        visible: timezoneArea.currentActionStateName == "addButton"

        ListView {
            id: addTimezoneListView
            width: parent.width
            height: listAreaMaxHeight
            anchors.top: parent.top
            anchors.left: parent.left
            clip: true

            property var selectedItemDict: new Object()

            function fillModel(){
                model.clear()
                for (var i = 0; i < zoneCodePair.length; i ++){
                    model.append({
                                  "timezone":zoneCodePair[i]["zones"][0],
                                  "cityList": getCityList(zoneCodePair[i]["zones"]),
                                  "offsetName": getOffsetName(zoneCodePair[i]["code"])
                                 })
                }
            }

            model: ListModel {}

            delegate: AddTimezoneItem{
                onSelectAction: {
                    if (isInUserTimezone(timezone)){
                        return
                    }

                    addTimezoneListView.selectedItemDict[timezone] = selected
                    var length = 0
                    for(var key in addTimezoneListView.selectedItemDict){
                        if(addTimezoneListView.selectedItemDict[key]){
                            length += 1
                        }
                    }
                    if(length > 0){
                        titleLine.rightLoader.item.addLabel = dsTr("Add")
                    }
                    else{
                        titleLine.rightLoader.item.addLabel = dsTr("Close")
                    }
                }
            }

            DScrollBar {
                flickable: addTimezoneListView
            }
        }
    }
}
