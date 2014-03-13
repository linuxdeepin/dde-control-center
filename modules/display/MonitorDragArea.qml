import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: monitorDragArea
    width: parent.width
    height: 220
    color: dconstants.contentBgColor
    clip: true

    property int adsorptionValue: 3

    property bool editable: false

    property int rootWindowWidth: rootWindow.displayWidth
    onRootWindowWidthChanged: {
        if(rootWindowWidth == 0){
            editable = false
        }
    }

    property var scaleFactorAndPadding: getScaleFactorAndPadding(openedMonitors)
    property var openedMonitors: {
        var openedM = new Array()
        for(var i in allMonitorsObjects){
            if(allMonitorsObjects[i].opened){
                openedM.push(allMonitorsObjects[i])
            }
        }
        return openedM
    }

    property var monitorsViews: {
        var views = new Array()
        for(var i=0; i<monitorItems.count; i++){
            views.push(monitorItems.itemAt(i))
        }
        return views
    }
    property var centerIdentifyWindows: {
        var wins = new Array()
        var component = Qt.createComponent("CenterIdentifyWindow.qml")
        for(var i in openedMonitors){
            var win = component.createObject(displayModule, { monitorObject: openedMonitors[i] })
            wins.push(win)
        }
        return wins
    }

    function getScaleFactorAndPadding(objs){
        var xPositionLeft = new Array()
        var xPositionRight = new Array()
        var yPositionUp = new Array()
        var yPositionDown = new Array()
        for(var i in objs){
            var obj = objs[i]
            xPositionLeft.push(obj.x)
            xPositionRight.push(obj.x + obj.width)
            yPositionUp.push(obj.y)
            yPositionDown.push(obj.y + obj.height)
        }
        var x1 = windowView.sortArray(xPositionLeft, false)[0]
        var x2 = windowView.sortArray(xPositionRight, true)[0]
        var y1 = windowView.sortArray(yPositionUp, false)[0]
        var y2 = windowView.sortArray(yPositionDown, true)[0]
        var real_width = x2 - x1
        var real_height = y2 - y1
        if(real_height/real_width < validArea.height/validArea.width){
            var scale_factor = validArea.width/real_width
            var y_padding = (validArea.height - scale_factor * real_height)/2
            return [scale_factor, 0-x1*scale_factor, y_padding-y1*scale_factor]
        }
        else{
            var scale_factor = validArea.height/real_height
            var x_padding = (validArea.width - scale_factor * real_width)/2
            return [scale_factor, x_padding-x1*scale_factor, 0-y1*scale_factor]
        }
    }

    function applyPostion(){
        var x_arr = new Array()
        var y_arr = new Array()
        for(var i in monitorsViews){
            var view = monitorsViews[i]
            x_arr.push(view.x)
            y_arr.push(view.y)
        }
        var min_x = arrayMin(x_arr)
        var min_y = arrayMin(y_arr)
        var factor = monitorsViews[0].monitorObject.width/monitorsViews[0].width
        for(var i in monitorsViews){
            var view = monitorsViews[i]
            var x_pos = (view.x - min_x) * factor
            var y_pos = (view.y - min_y) * factor
            view.monitorObject.SetPos(x_pos, y_pos)
        }
    }

    function resetPosition(){
        var myScaleFactorAndPadding = getScaleFactorAndPadding(monitorsViews)
        for(var i in monitorsViews){
            var view = monitorsViews[i]
            view.x = view.x * myScaleFactorAndPadding[0] + myScaleFactorAndPadding[1]
            view.y = view.y * myScaleFactorAndPadding[0] + myScaleFactorAndPadding[2]
            view.width = view.width * myScaleFactorAndPadding[0]
            view.height = view.height * myScaleFactorAndPadding[0]
        }
    }

    function presentCurrent(index){
        for(var i=0; i<monitorItems.count; i++){
            var monitorItem = monitorItems.itemAt(i)
            monitorItem.z = index == i ? 10 : 0
        }
    }

    function closeToView(bView, currentView){
        var positionCode = getRelativePosition(bView, currentView)
        switch(positionCode){
            case "right":
            currentView.x = bView.x2;
            if((currentView.y + currentView.height) < bView.y){
                currentView.y = bView.y - currentView.height
            }
            else if(currentView.y > (bView.y + bView.height)){
                currentView.y = bView.y + bView.height
            }
            if(Math.abs(currentView.y - bView.y) < adsorptionValue){
                currentView.y = bView.y
            }
            if(Math.abs(currentView.y2 - bView.y2) < adsorptionValue){
                currentView.y = bView.y2 - currentView.height
            }
            break;

            case "left":
            currentView.x = bView.x - currentView.width;
            if(currentView.y2 < bView.y){
                currentView.y = bView.y - currentView.height
            }
            else if(currentView.y > bView.y2){
                currentView.y = bView.y + bView.height
            }
            if(Math.abs(currentView.y - bView.y) < adsorptionValue){
                currentView.y = bView.y
            }
            if(Math.abs(currentView.y2 - bView.y2) < adsorptionValue){
                currentView.y = bView.y2 - currentView.height
            }
            break;

            case "top": 
            currentView.y = bView.y - currentView.height; 
            if(currentView.x2 < bView.x){
                currentView.x = bView.x - currentView.width
            }
            else if(currentView.x > bView.x2){
                currentView.x = bView.x + bView.width
            }
            if(Math.abs(currentView.x - bView.x) < adsorptionValue){
                currentView.x = bView.x
            }
            if(Math.abs(currentView.x2 - bView.x2) < adsorptionValue){
                currentView.x = bView.x2 - currentView.width
            }
            break;

            case "bottom": 
            currentView.y = bView.y2; 
            if(currentView.x2 < bView.x){
                currentView.x = bView.x - currentView.width
            }
            else if(currentView.x > bView.x2){
                currentView.x = bView.x + bView.width
            }
            if(Math.abs(currentView.x - bView.x) < adsorptionValue){
                currentView.x = bView.x
            }
            if(Math.abs(currentView.x2 - bView.x2) < adsorptionValue){
                currentView.x = bView.x2 - currentView.width
            }
            break;
        }
    }

    function arrayMin(a){
        var minIndex = 0
        if(a.length > 1){
            for(var i=1; i<a.length; i++){
                if(a[minIndex] > a[i]){
                    minIndex = i
                }
            }
        }
        return minIndex
    }

    function getDistance(x1, y1, x2, y2){
        return Math.sqrt(Math.pow((x2-x1), 2) + Math.pow((y2-y1), 2))
    }

    function getAngle(x1, y1, x2, y2){
        return Math.atan2(x2-x1, y2-y1) + Math.PI
    }

    function isInside(a_view, b_view){
        var b_center_x = b_view.x + (b_view.x2 - b_view.x)/2
        var b_center_y = b_view.y + (b_view.y2 - b_view.y)/2
        if(b_center_x >= a_view.x && b_center_x <= a_view.x2 && b_center_y >= a_view.y && b_center_y <= a_view.y2){
            return true
        }
        else{
            return false
        }
    }

    function getViewDistance(aView, bView){
        var c_x1 = Math.max(aView.x1, bView.x1)
        var c_x2 = Math.min(aView.x2, bView.x2)
        var c_y1 = Math.max(aView.y1, bView.y1)
        var c_y2 = Math.min(aView.y2, bView.y2)

        if(c_x1 <= c_x2 && c_y1 <= c_y2){
            return 0
        }
        else if(c_x1 <= c_x2 && c_y1 > c_y2){
            return c_y1 - c_y2
        }
        else if(c_x1 > c_x2 && c_y1 <= c_y2){
            return c_x1 - c_x2
        }
        else{
            return getDistance(c_x1, c_y2, c_x2, c_y2)
        }
    }

    function getRelativePosition(a_view, b_view){
        var b_center_x = b_view.x + (b_view.x2 - b_view.x)/2
        var b_center_y = b_view.y + (b_view.y2 - b_view.y)/2
        if(b_center_x >= a_view.x && b_center_x <= a_view.x2 && b_center_y >= a_view.y && b_center_y <= a_view.y2){
            return "inside"
        }
        else{
            var a_center_x = a_view.x1 + (a_view.x2 - a_view.x1)/2
            var a_center_y = a_view.y1 + (a_view.y2 - a_view.y1)/2
            var angle_1 = getAngle(a_center_x, a_center_y, a_view.x1, a_view.y1)
            var angle_2 = getAngle(a_center_x, a_center_y, a_view.x1, a_view.y2)
            var angle_3 = getAngle(a_center_x, a_center_y, a_view.x2, a_view.y2)
            var angle_4 = getAngle(a_center_x, a_center_y, a_view.x2, a_view.y1)
            var relativeAngle = getAngle(a_center_x, a_center_y, b_center_x, b_center_y)
            if(relativeAngle <= angle_1){
                return "top"
            }
            else if(relativeAngle <=angle_2){
                return "left"
            }
            else if(relativeAngle <= angle_3){
                return "bottom"
            }
            else if(relativeAngle <= angle_4){
                return "right"
            }
            else{
                return "top"
            }
        }
    }

    function isInArray(m, arr){
        for(var i=0; i<arr.length; i++){
            if(m == arr[i]){
                return true
            }
        }
        return false
    }

    function getNeighbors(currentView){
        var neighbors = new Array()
        var fatherViews = new Array()
        var childrenViews = new Array()

        for(var i=0; i<monitorItems.count; i++){
            var view = monitorItems.itemAt(i)
            if(view == currentView){
                fatherViews.push(view)
            }
            else{
                childrenViews.push(view)
            }
        }

        while(fatherViews.length > 0){
            var tempFather = new Array()
            for(var i in fatherViews){
                var f = fatherViews[i]
                var tempChildren = new Array()
                for(var j in childrenViews){
                    var c = childrenViews[j]
                    if(getViewDistance(c, f) == 0){
                        tempFather.push(c)
                    }
                    else{
                        tempChildren.push(c)
                    }
                }
                neighbors.push(f)
                childrenViews = tempChildren
            }
            fatherViews = tempFather
        }
        return [neighbors, childrenViews]
    }

    function getNearestView(currentView, farAwayViews){
        var distances = new Array()
        for(var i in farAwayViews){
            var distance = getViewDistance(currentView, farAwayViews[i])
            distances.push(distance)
        }
        var nearestIndex = arrayMin(distances)
        return farAwayViews[nearestIndex]
    }

    function doRelease(index){
        var currentView = monitorItems.itemAt(index)

        // lookup the beJoined view or beOverlapped view
        var farAwayViews = new Array()
        var farAwayViewsDistance = new Array()
        for(var i=0; i<monitorItems.count; i++){
            if(i !=index ){
                var bView = monitorItems.itemAt(i)
                if(bView.beJoined){
                    timeoutResetAllPosition.restart()
                    displayId.JoinMonitor(currentView.monitorObject.name, bView.monitorObject.name)
                    return
                }
                else if(bView.beOverlapped){
                    closeToView(bView, currentView)
                }
            }
        }

        var infos = getNeighbors(currentView)
        var currentViewNeighbors = infos[0]
        var farAwayViews = infos[1]

        // if 1 monitor is far away from others, it need close to nearest View firstly.
        if(currentViewNeighbors.length == 1){
            var nearestView = getNearestView(currentViewNeighbors[0], farAwayViews)
            closeToView(nearestView, currentViewNeighbors[0])
        }

        resetAllPosition(currentView)

    }

    function doDrag(index){
        var currentView = monitorItems.itemAt(index)
        for(var i=0; i<monitorItems.count; i++){
            if(i !=index ){
                var bView = monitorItems.itemAt(i)
                var c_x1 = Math.max(currentView.x1, bView.x1)
                var c_x2 = Math.min(currentView.x2, bView.x2)
                var c_y1 = Math.max(currentView.y1, bView.y1)
                var c_y2 = Math.min(currentView.y2, bView.y2)
                if(c_x1 <= c_x2 && c_y1 <= c_y2){
                    var c_area = (c_x2-c_x1)*(c_y2-c_y1)
                    var b_area = (bView.x2-bView.x1)*(bView.y2-bView.y1)
                    var cur_area = currentView.width*currentView.height
                    if(c_area >= cur_area/2 || c_area >= b_area/2 || isInside(bView, currentView)){
                        bView.beJoined = true
                        bView.beOverlapped = false
                    }
                    else{
                        bView.beOverlapped = true
                        bView.beJoined = false
                    }
                }
                else{
                    bView.beJoined = false
                    bView.beOverlapped = false
                }
            }
        }
    }

    function resetAllPosition(currentView){
        var infos = getNeighbors(currentView)
        var currentViewNeighbors = infos[0]
        var farAwayViews = infos[1]

        for(var i in farAwayViews){
            var nearestView = getNearestView(farAwayViews[i], currentViewNeighbors)
            closeToView(nearestView, farAwayViews[i])
            currentViewNeighbors.push(farAwayViews[i])
        }
        resetPosition()
    }

    Timer{
        id: timeoutResetAllPosition
        repeat: false
        running: false
        interval: 200
        onTriggered: {
            resetAllPosition(monitorsViews[0])
        }
    }

    Item {
        id: buttonArea
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: editButton.height
        visible: {
            if(openedMonitors.length == 1){
                return openedMonitors[0].isComposited
            }
            else{
                return true
            }
        }

        Row{
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: !editable

            DTextButton {
                text: dsTr("Identify")
                onClicked: {
                    for(var i in centerIdentifyWindows){
                        centerIdentifyWindows[i].showWindow()
                    }
                }
            }

            DTextButton {
                id: editButton
                text: dsTr("Edit")
                onClicked: {
                    editable = true
                    for(var i=0; i<monitorsViews.length; i++){
                        monitorsViews[i].identifyWindow.show()
                    }
                }
            }
        }
        
        Row {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: editable

            DTextButton {
                text: dsTr("Apply")
                onClicked: {
                    applyPostion()
                    editable = false
                    displayChangesApply()
                }
            }

            DTextButton {
                text: dsTr("Cancel")
                onClicked: {
                    editable = false
                    for(var i in monitorsViews){
                        var view = monitorsViews[i]
                        view.reset()
                    }
                }
            }
        }

    }

    Rectangle{
        id: validArea
        anchors.centerIn: parent
        height: parent.height - buttonArea.height * 2
        width: parent.width - 40
        color: Qt.rgba(1, 0, 0, 0)

        Repeater{
            id: monitorItems
            model: openedMonitors.length
            delegate: MonitorComponent{
                monitorObject: openedMonitors[index]
                scaleFactorAndPadding: monitorDragArea.scaleFactorAndPadding
                inEditMode: monitorDragArea.editable
                openedMonitorNumber: openedMonitors.length

                onPressedAction: {
                    presentCurrent(monitorIndex)
                }

                onReleasedAction: {
                    doRelease(monitorIndex)
                }

                onDragAndMoveAction: {
                    doDrag(monitorIndex)
                }
            }
        }
    }
}
