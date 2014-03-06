import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: monitorDragArea
    width: parent.width
    height: 180
    color: dconstants.contentBgColor
    clip: true

    property int padding: 10
    property var monitorComponent: Qt.createComponent("MonitorComponent.qml")
    property var scaleFactorAndPadding: getScaleFactorAndPadding()
    property var monitorViews: new Array()

    property var openedMonitors: {
        var openedM = new Array()
        for(var i in allMonitorsObjects){
            if(allMonitorsObjects[i].opened){
                openedM.push(allMonitorsObjects[i])
            }
        }
        print("==> [debug] primary:", displayId.primary)
        return openedM
    }

    function getScaleFactorAndPadding(){
        var xPositionLeft = new Array()
        var xPositionRight = new Array()
        var yPositionUp = new Array()
        var yPositionDown = new Array()
        for(var i in openedMonitors){
            var obj = openedMonitors[i]
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
            return [scale_factor, 0, y_padding]
        }
        else{
            var scale_factor = validArea.height/real_height
            var x_padding = (validArea.width - scale_factor * real_width)/2
            return [scale_factor, x_padding, 0]
        }
    }

    function presentCurrent(index){
        for(var i=0; i<monitorItems.count; i++){
            var monitorItem = monitorItems.itemAt(i)
            monitorItem.z = index == i ? 10 : 0
        }
    }

    function releaseAction(index){
        var currentView = monitorItems.itemAt(index)
        for(var i=0; i<monitorItems.count; i++){
            if(i !=index ){
                var bView = monitorItems.itemAt(i)
                if(bView.beJoined){
                    displayId.JoinMonitor(currentView.monitorObject.name, bView.monitorObject.name)
                }
            }
        }
    }

    function dragAction(index){
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
                    if(c_area >= cur_area/2 || c_area >= b_area/2){
                        bView.beJoined = true
                        return 
                    }
                }
                bView.beJoined = false
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
        width: parent.width - 20
        color: Qt.rgba(1, 0, 0, 0)

        Repeater{
            id: monitorItems
            model: openedMonitors.length
            delegate: MonitorComponent{
                monitorObject: openedMonitors[index]
                scaleFactorAndPadding: monitorDragArea.scaleFactorAndPadding
            }
        }
    }
}
