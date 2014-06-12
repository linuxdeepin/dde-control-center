/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
                            dbusThemeManager.Set("fontsize", "", parseInt(slider.value))
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
