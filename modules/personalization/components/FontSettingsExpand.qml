import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: font_themes_expand

    content.sourceComponent:  DCenterLine {
        height: 60
        color: dconstants.contentBgColor
        centerPadding: 20
        leftWidth: 60
        title.text: dsTr("Size")
        content.sourceComponent: Row {
            spacing: 10
            FontSlider{
                id: slider
                width: 160
                maximumValue: 16
                minimumValue: 9
                value: currentThemeObject.fontSize
                stepSize: 1

                onPressedChanged: {
                    if(!pressed){
                        if(currentThemeObject.fontSize != parseInt(slider.value)){
                            dbusThemeManager.SetFontSize(parseInt(slider.value))
                        }
                    }
                }
            }

            DssH3 {
                anchors.verticalCenter: parent.verticalCenter
                property int fontSize: {
                    var new_value = parseInt(slider.value)
                    if((slider.value - new_value) >= 0.5){
                        new_value += 1
                    }
                    return new_value
                }
                text: "Font %1".arg(fontSize)
                font.pixelSize: fontSize
            }
        }
    }
}
