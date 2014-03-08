import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: monitorDragArea
    width: parent.width
    height: 180
    color: dconstants.contentBgColor
    clip: true

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

    property var lastComponentInfo: new Array()
    property var monitorsViews: {
        var views = new Array()
        for(var i=0; i<monitorItems.count; i++){
            views.push(monitorItems.itemAt(i))
        }
        return views
    }

    function recordLastComponentInfo(){
        lastComponentInfo = new Array()
        for(var i=0; i<monitorItems.count; i++){
            var info = {}
            var item = monitorItems.itemAt(i)
            info["x"] = item.x
            info["y"] = item.y
            info["width"] = item.width
            info["height"] = item.height
            lastComponentInfo.push(info)
        }
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

    function doRelease(index){
        var currentView = monitorItems.itemAt(index)
        for(var i=0; i<monitorItems.count; i++){
            if(i !=index ){
                var bView = monitorItems.itemAt(i)
                if(bView.beJoined){
                    displayId.JoinMonitor(currentView.monitorObject.name, bView.monitorObject.name)
                    return 
                }
                else if(bView.beOverlapped){
                    var positionCode = getRelativePosition(bView, currentView)
                    switch(positionCode){
                        case "right": currentView.x = bView.x2; resetPosition(); break;
                        case "left": currentView.x = bView.x - currentView.width; resetPosition(); break;
                        case "top": currentView.y = bView.y - currentView.height; resetPosition(); break;
                        case "bottom": currentView.y = bView.y2; resetPosition(); break;
                    }
                }
                else{
                    var positionCode = getRelativePosition(bView, currentView)
                    switch(positionCode){
                        case "right": currentView.x = bView.x2; resetPosition(); break;
                        case "left": currentView.x = bView.x - currentView.width; resetPosition(); break;
                        case "top": currentView.y = bView.y - currentView.height; resetPosition(); break;
                        case "bottom": currentView.y = bView.y2; resetPosition(); break;
                    }
                }
            }
        }
    }

    function getAngle(x1, y1, x2, y2){
        return Math.atan2(x2-x1, y2-y1) + Math.PI
    }

    function getRelativePosition(a_view, b_view){
        var b_center_x = b_view.x1 + (b_view.x2 - b_view.x1)/2
        var b_center_y = b_view.y1 + (b_view.y2 - b_view.y1)/2
        if(b_center_x >= a_view.x1 && b_center_x <= a_view.x2 && b_center_y >= a_view.y1 && b_center_y <= a_view.y2){
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

    function doDrag(index){
        var currentView = monitorItems.itemAt(index)
        for(var i=0; i<monitorItems.count; i++){
            if(i !=index ){
                var bView = monitorItems.itemAt(i)
                var c_x1 = Math.max(currentView.x1, bView.x1)
                var c_x2 = Math.min(currentView.x2, bView.x2)
                var c_y1 = Math.max(currentView.y1, bView.y1)
                var c_y2 = Math.min(currentView.y2, bView.y2)
                if(c_x1 < c_x2 && c_y1 < c_y2){
                    var c_area = (c_x2-c_x1)*(c_y2-c_y1)
                    var b_area = (bView.x2-bView.x1)*(bView.y2-bView.y1)
                    var cur_area = currentView.width*currentView.height
                    if(c_area >= cur_area/2 || c_area >= b_area/2 || getRelativePosition(bView, currentView) == "inside"){
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

    DOpacityImageButton{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        source: "images/edit.png"
    }

    Rectangle{
        id: validArea
        anchors.centerIn: parent
        height: parent.height - 40
        width: parent.width - 40
        color: Qt.rgba(1, 0, 0, 0)

        Repeater{
            id: monitorItems
            model: openedMonitors.length
            delegate: MonitorComponent{
                monitorObject: openedMonitors[index]
                scaleFactorAndPadding: monitorDragArea.scaleFactorAndPadding
                inEditMode: true

                onPressedAction: {
                    recordLastComponentInfo()
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
