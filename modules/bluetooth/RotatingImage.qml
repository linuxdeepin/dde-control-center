import QtQuick 2.1

Image {
    id: img
    
    smooth: true
    source: "images/refresh.png"
    
    property bool rotating: false
    
    onRotatingChanged: rotating_animation.running = rotating
    
    PropertyAnimation {
        id: rotating_animation
        target: img
        property: "rotation"
        from: 0
        to: 360
        duration: 1000
        loops: Animation.Infinite        
        easing.type: Easing.OutQuad
    }
}