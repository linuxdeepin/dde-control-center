import QtQuick 2.0
import Deepin.Widgets 1.0

Rectangle {
    id: monitorDragArea
    width: parent.width 
    height: 180
    color: dconstants.contentBgColor
    clip: true

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

    function releaseAction(index){
        var currentView = monitorViews[index]
        if(outputObjectsOpened.length == 1){
            currentView.x = (width - currentView.width)/2
            currentView.y = (height - currentView.height)/2
        }
    }
}
