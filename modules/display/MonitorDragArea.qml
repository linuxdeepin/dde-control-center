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

    property var monitorViews: {
        var views = new Array()
        for(var i=0; i<outputObjects.length;i++){
            var view = monitorComponent.createObject(monitorDragArea, {
                "outputObj": outputObjects[i],
                "monitorIndex": i
            })
            views.push(view)
        }
        return views
    }

    Component.onCompleted: {
        if(outputObjectsOpened.length == 1){
            releaseAction(outputObjectsOpened[0][1])
        }
        else{
            var xPositionLeft = new Array()
            var xPositionRight = new Array()
            var yPositionUp = new Array()
            var yPositionDown = new Array()
            for(var i in outputObjectsOpened){
                var allocation = outputObjectsOpened[i][0].allocation
                xPositionLeft.push(allocation[0])
                xPositionRight.push(allocation[0] + allocation[2])
                yPositionUp.push(allocation[1])
                yPositionDown.push(allocation[1] + allocation[3])
            }
            var x1 = windowView.sortArray(xPositionLeft, false)[0]
            var x2 = windowView.sortArray(xPositionRight, true)[0]
            var y1 = windowView.sortArray(yPositionUp, false)[0]
            var y2 = windowView.sortArray(yPositionDown, true)[0]
            print(x1, x2, y1, y2)
        }
    }

    function presentCurrent(index){
        monitorViews[index].z = 10
        for(var i in monitorViews){
            if(monitorViews[i].z == 10 && i != index){
                monitorViews[i].z = 0
            }
        }
    }

    function releaseAction(index){
        var currentView = monitorViews[index]
        if(outputObjectsOpened.length == 1){
            var modeInfo = currentView.outputObj.mode
            currentView.width = width - 60
            currentView.height = currentView.width * modeInfo[2]/modeInfo[1]
            currentView.x = (width - currentView.width)/2
            currentView.y = (height - currentView.height)/2
        }
        else{
        }
    }
}
