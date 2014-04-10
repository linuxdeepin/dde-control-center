import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: font_themes_expand

    content.sourceComponent:  DCenterLine {
        height: 50
        color: dconstants.contentBgColor
        centerPadding: 20
        leftWidth: 70
        title.text: dsTr("Size")
        content.sourceComponent: DSliderEnhanced {
            width: 190

            min: 9
            max: 16
            init: 11
            rulerVisible: false

            onValueConfirmed:{
                print(value)
            }

            onValueChanged: {
                var newValue = parseInt(value)
                if(newValue != value){
                    if(value - newValue >= 0.5){
                        newValue += 1
                    }
                    setValue(newValue, false)
                }
            }

            Component.onCompleted: {
                addRuler(9, "")
                addRuler(10, "")
                addRuler(11, "")
                addRuler(12, "")
                addRuler(13, "")
                addRuler(14, "")
                addRuler(15, "")
                addRuler(16, "")
            }
        }
    }
}
