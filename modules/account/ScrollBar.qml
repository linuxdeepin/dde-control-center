import QtQuick 2.1

Rectangle {
      property variant flickable: parent

      property bool vertical: true
      property bool hideOnStop: true

      property int scrollBarWidth: 6
      property int scrollBarRadius: 3
      
      property real onMouseEnterOpacity: 0.3

      color: "black"
      radius: scrollBarRadius

      function opacityLambda()
      {
          if (!hideOnStop) {
              return onMouseEnterOpacity;
          }

          return (flickable.flicking || flickable.moving) ? (vertical ? (height >= parent.height ? 0 : onMouseEnterOpacity) : (width >= parent.width ? 0 : onMouseEnterOpacity)) : 0;
      }

      opacity: 0.5 //opacityLambda()

      width: vertical ? scrollBarWidth : flickable.visibleArea.widthRatio * parent.width
      height: vertical ? flickable.visibleArea.heightRatio * parent.height : scrollBarWidth
      x: vertical ? parent.width - width : flickable.visibleArea.xPosition * parent.width
      y: vertical ? flickable.visibleArea.yPosition * parent.height : parent.height - height

      Behavior on opacity { NumberAnimation { duration: 200 }}
}