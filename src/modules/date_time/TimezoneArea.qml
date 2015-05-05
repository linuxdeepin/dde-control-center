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
        PropertyAnimation { easing.type: Easing.InQuad; duration: 150 }
    }

    signal hasDSTList(bool sHasDST ,var sDSTList)
    signal isAddState(bool state)
    property bool isInAddState:false

    property var allZoneInfo: {
        var zoneInfos = new Array()
        var allZones = gDate.GetZoneList()
        for (var i in allZones) {
            var info = gDate.GetZoneInfo(allZones[i])
            zoneInfos.push(info)
        }

        return zoneInfos
    }

    property var timezoneInformation: {
        var zoneList = new Array()
        var cityList = new Array()
        var codeList = new Array()
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
    property var userTimezoneOffsetList: {
        var tmpArray = new Array()
        for (var i = 0; i < userTimezoneList.length; i ++){
            var tmpOffset = getOffsetByZone(userTimezoneList[i])
            if (tmpArray.indexOf(tmpOffset) == -1){//not exit,add it,DST is the same offset
                tmpArray.push(tmpOffset)
            }
        }

        return tmpArray
    }

    property var currentTimezone: gDate.timezone

    property var currentTimezoneOffset: getOffsetByZone(currentTimezone)

    property string currentActionStateName: ""
    property var listModelComponent: DListModelComponent {}

    property int listAreaMaxHeight

    property bool inDeleteAction: false

    function isInUserTimezone(offset){
        if (userTimezoneOffsetList.indexOf(offset) != -1)
            return true
        else
            return false
    }

    function getCityByZone(zone){
        if (zone == ""){
            print ("[Info](getCityByZone) Zone is empty!")
            return ""
        }
        else{
            if (!gDate.GetZoneInfo(zone)){
                print ("[Info](getCityByZone) Get zone info failed!",zone)
                return ""
            }
            return gDate.GetZoneInfo(zone)[1]
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
            if (gDate.GetZoneInfo(tmpArray[i])[3][0] == 0 || gDate.GetZoneInfo(tmpArray[i])[3][1] == 0){
                DSTArray[0] = tmpArray[i]
                continue
            }
            else{
                DSTArray[gDate.GetZoneInfo(tmpArray[i])[3][2] - tmpOffset] = tmpArray[i]
            }
        }


        return DSTArray
    }

    function getOffsetByZone(zone){
        if (zone == ""){
            print ("[Info](getOffsetByZone) Zone is empty!")
            return ""
        }
        else{
            if (!gDate.GetZoneInfo(zone)){
                print ("[Info](getOffsetByZone) Get zone info failed!",zone)
                return ""
            }
            return gDate.GetZoneInfo(zone)[2]
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
                tmpList += gDate.GetZoneInfo(cityArray[i])[1] + ", "
            }

            if (gDate.GetZoneInfo(cityArray[cityArray.length - 1]))
                tmpList += gDate.GetZoneInfo(cityArray[cityArray.length - 1])[1]

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

    function getTimezoneListByOffset(offset){
        for (var i = 0; i < zoneCodePair.length; i ++){
            if (zoneCodePair[i]["code"] == offset){
                return zoneCodePair[i]["zones"]
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

    //Data may not be updated immediately
    onUserTimezoneListChanged: {
        delayUpdateTimer.stop()
        delayUpdateTimer.start()
    }

    //Avoid show tooltip Inadvertently
    onHeightChanged: {
        if (titleLine.rightLoader.item != null)
            titleLine.rightLoader.item.hideTooltip()
    }

    Timer {
        id:delayUpdateTimer
        repeat: false
        interval: 300
        running: false
        onTriggered: {
            userTimezoneListView.updateModel()
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

                    addTimezoneListView.selectedItemDict = {}

                    timeZoneArea.isAddState(false)
                    timeZoneArea.isInAddState = false
                }
                else if (timezoneArea.currentActionStateName == "deleteButton"){
                    inDeleteAction = false
                }

                if (currentActionStateName == "deleteButton"){
                    inDeleteAction = true
                }

                // before changed

                timezoneArea.currentActionStateName = currentActionStateName

                // after changed
                if(timezoneArea.currentActionStateName == "addButton"){
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

            function updateModel(){
                if (userTimezoneOffsetList.length > model.count){
                    addToModel()
                }
                else if (userTimezoneOffsetList.length < model.count){
                    deleteFromModel()
                }
            }

            function deleteFromModel(){
                var tmpList = new Array()

                var modelCount = model.count
                for (var i = 0; i < model.count; i ++){//get all invalid item
                    var tmpZone = model.get(i).timezone
                    if (!isInUserTimezoneList(tmpZone))
                        tmpList.push(tmpZone)
                }

                for (i = 0; i < tmpList.length; i ++){//delete all invalid item from model
                    model.remove(getItemIndex(tmpList[i]))
                    addTimezoneListView.addToModel(tmpList[i])
                }
            }

            function addToModel(){
                for (var i = 0; i < userTimezoneOffsetList.length; i ++){
                    if (!isInModel(userTimezoneOffsetList[i])){
                        model.append({
                                         "timezone" : getZoneByOffset(userTimezoneOffsetList[i])
                                     })
                        addTimezoneListView.deleteFromMode(userTimezoneOffsetList[i])
                    }
                }
            }

            function getItemIndex(timezone){
                var tmpOffset = getOffsetByZone(timezone)

                for (var i = 0 ; i < model.count; i ++){
                    if (getOffsetByZone(model.get(i).timezone) == tmpOffset)
                        return i
                }

                return -1
            }

            function isInModel(timezoneOffset){
                for (var i = 0 ; i < model.count; i ++){
                    if (getOffsetByZone(model.get(i).timezone) == timezoneOffset)
                        return true
                }

                return false
            }

            function isInUserTimezoneList(timezone){
                var tmpOffset = getOffsetByZone(timezone)
                if (userTimezoneOffsetList.indexOf(tmpOffset) < 0){
                    return false
                }
                else{
                    return true
                }
            }

            model: ListModel {}

            delegate: UserTimezoneItem{
                onDeleteAction: {
                    print ("==> Deletting usertimezone...",timezoneOffset)
                    var tmpCityList = new Array()
                    for (var i = 0; i < userTimezoneList.length; i ++){
                        if (getOffsetByZone(userTimezoneList[i]) == timezoneOffset)
                            tmpCityList.push(userTimezoneList[i])
                    }

                    for (var i = 0; i < tmpCityList.length; i ++){
                        gDate.DeleteUserTimezone(tmpCityList[i])
                    }
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
                    var zones = zoneCodePair[i]["zones"]
                    var offset = zoneCodePair[i]["code"]
                    model.append({
                                     "timezone":zones[0],
                                     "timeOffset":offset
                                 })
                }
            }

            function addToModel(timezone){
                var tmpOffset = getOffsetByZone(timezone)
                var tmpIndex = 0

                //get index to sort the model
                for (var i = 0; i < model.count; i ++){
                    if (model.get(i).timeOffset > tmpOffset){
                        tmpIndex = i
                        break
                    }
                }

                model.insert(tmpIndex, {
                                 "timezone":timezone,
                                 "timeOffset":tmpOffset
                             })
            }

            function deleteFromMode(timezoneOffset){
                model.remove(getItemIndex(timezoneOffset))
            }

            function getItemIndex(offset){
                for (var i = 0 ; i < model.count; i ++){
                    if (getOffsetByZone(model.get(i).timezone) == offset)
                        return i
                }

                return -1
            }

            model: ListModel {}

            delegate: AddTimezoneItem{
                onSelectAction: {
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

            Component.onCompleted: {
                fillModel()
            }
        }
    }
}
