import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: monitorDragArea
    width: parent.width
    height: 180
    color: dconstants.contentBgColor
    clip: true

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
        color: Qt.rgba(1, 0, 0, 0.1)

        Repeater{
            id: monitorItems
            model: {
                var myModel = listModelComponent.createObject(monitorItems, {})
                print(scaleFactorAndPadding)
                //for(var i in allMonitorsObjects){
                    var obj = {"name": "1", "x": 0,"y": 0, "width": 1366, "height": 768}
                    myModel.append({
                        "monitorObj": obj,
                        "scaleFactorAndPadding": scaleFactorAndPadding
                    })
                //}
                return myModel
            }
            delegate: MonitorComponent{}
        }
    }

    property int padding: 10
    property var monitorComponent: Qt.createComponent("MonitorComponent.qml")
    property var scaleFactorAndPadding: getScaleFactorAndPadding()
    property var monitorViews: new Array()

    function monitorChanged(all){
        print(scaleFactorAndPadding)
        for(var i in monitorViews){
            monitorViews[i].destroy()
        }
        if(all){
            for(var i=0; i<all.length;i++){
                var view = monitorComponent.createObject(validArea, {
                    "outputObj": all[i],
                    "monitorIndex": i,
                    "scaleFactor": scaleFactorAndPadding[0],
                    "xPadding": scaleFactorAndPadding[1],
                    "yPadding": scaleFactorAndPadding[2]
                })
                monitorViews.push(view)
            }
        }
    }

    property var outputObjectsOpened: {
        var openedMonitors = new Array()
        for(var i in allMonitorsObjects){
            if(allMonitorsObjects[i].opened){
                openedMonitors.push(allMonitorsObjects[i])
            }
        }
        return openedMonitors
    }

    Component.onCompleted: {
        if(outputObjectsOpened.length == 1){
            //releaseAction(0)
        }
        else{
        }
    }

    function getScaleFactorAndPadding(){
        var xPositionLeft = new Array()
        var xPositionRight = new Array()
        var yPositionUp = new Array()
        var yPositionDown = new Array()
        for(var i in outputObjectsOpened){
            var obj = outputObjectsOpened[i]
            xPositionLeft.push(obj.x)
            xPositionRight.push(obj.x + obj.width)
            yPositionUp.push(obj.y)
            yPositionDown.push(obj.y + obj.height)
        }
        var x1 = windowView.sortArray(xPositionLeft, false)[0]
        var x2 = windowView.sortArray(xPositionRight, true)[0]
        var y1 = windowView.sortArray(yPositionUp, false)[0]
        var y2 = windowView.sortArray(yPositionDown, true)[0]
        if(y2-y1/x2-x1 > validArea.height/validArea.width){
            var scale_factor = validArea.width/(x2-x1)
            var y_padding = (validArea.height - scale_factor * (y2-y1))/2
            return [scale_factor, 0, y_padding]
        }
        else{
            var scale_factor = validArea.height/(y2-y1)
            var x_padding = (validArea.width - scale_factor * (x2-x1))/2
            return [scale_factor, x_padding, 0]
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
            var modeInfo = currentView.outputObj.currentMode
            currentView.width = width - 100
            currentView.height = currentView.width * modeInfo[2]/modeInfo[1]
            currentView.x = (width - currentView.width)/2
            currentView.y = (height - currentView.height)/2
        }
        else{
        }
    }
}
