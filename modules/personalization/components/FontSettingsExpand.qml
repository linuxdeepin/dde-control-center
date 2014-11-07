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
    property var standardFontList: dbusThemeManager.fontNameList
    property var monoFontList:dbusThemeManager.fontMonoList
    property int contenHeight: 35
    property int contenTitleWidth: 80
    property int contenCenterPadding: 20
    property int comboboxWidth: 200


    Column {
        height: contenHeight * 3
        width: parent.width

        FontComboBoxLine {
            parentWindow: rootWindow
            height: contenHeight
            leftWidth: contenTitleWidth
            centerPadding: contenCenterPadding
            title.text: dsTr("Standard")
            boxWidth: comboboxWidth
            defaultIndex: standardFontList.indexOf(currentThemeObject.fontName)
            menuLabels: standardFontList

            onMenuSelect: {
                if (currentThemeObject.fontName != standardFontList[index])
                    dbusThemeManager.Set("font-name", standardFontList[index])
            }
        }


        FontComboBoxLine {
            parentWindow: rootWindow
            height: contenHeight
            leftWidth: contenTitleWidth
            centerPadding: contenCenterPadding
            title.text: dsTr("Monospaced")
            boxWidth: comboboxWidth
            defaultIndex: monoFontList.indexOf(currentThemeObject.fontMono)
            menuLabels: monoFontList

            onMenuSelect: {
                if (currentThemeObject.fontMono != monoFontList[index])
                    dbusThemeManager.Set("font-mono", monoFontList[index])
            }
        }

        DCenterLine {
               height: contenHeight
               centerPadding: 10
               leftWidth: 80
               title.text: dsTr("Size")
               content.sourceComponent: Row {
                   spacing: 5
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
                                   dbusThemeManager.Set("font-size", parseInt(slider.value))
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
                       text: dsTr("Font %1").arg(fontSize)
                       font.pixelSize: fontSize
                   }
               }
           }
    }

}
