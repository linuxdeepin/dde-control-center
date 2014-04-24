import QtQuick 2.0

Item {
    property alias model: view.model
    property alias delegate: view.delegate
    property alias currentIndex: view.currentIndex

    property real itemWidth: 30
    property real itemHeight: 30
    property alias view: view

    PathView {
        id: view
        anchors.fill: parent
        path: Path {
            startX: view.x + itemWidth/2
            startY: view.y + view.height/2
            PathLine { relativeX: view.width; relativeY: 0 }
        }
    }
}
