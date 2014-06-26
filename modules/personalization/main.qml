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
import DBus.Com.Deepin.Daemon.ThemeManager 1.0
import "components"
import "widgets"

Item {
    id: personalizationModule
    anchors.fill: parent

    Connections{
        target: rootWindow

        onModuleStartChange: {
            rootWindow.clickedToHide = true
        }

        onPanelHided: {
            rootWindow.clickedToHide = true
        }
    }

    property int cellWidth: 144
    property int cellHeight: 112

    property var constants: DConstants {}
    property var listModelComponent: DListModelComponent {}

    property int activeExpandIndex: 0

    property var dbusThemeManager: ThemeManager{}
    property var themeObjectList: {
        var themes = new Array()
        var themeList = dbusThemeManager.themeList
        for(var i in themeList){
            var themeObj = themeComponent.createObject(personalizationModule, { path: themeList[i] })
            themes.push(themeObj)
        }
        return themes
    }
    property var currentThemeObject: {
        for(var i in themeObjectList){
            var obj = themeObjectList[i]
            if(obj.name == dbusThemeManager.currentTheme){
                return obj
            }
        }
        return themeObjectList[0]
    }

    property var previewsWindow: PreviewWindow {
        x: rootWindow.x - width
        y: rootWindow.y + 18
        themeObject: currentThemeObject
    }

    Component {
        id: themeComponent
        Theme {}
    }

    Column {
        id: contentArea
        width: parent.width
        height: parent.height

        property int maxConetentHeight: height - personalizationTitle.height -  32 * 5 - 2

        DssTitle {
            id: personalizationTitle
            text: modulesId.moduleLocaleNames["personalization"]
        }

        DSeparatorHorizontal {}

        ThemeExpand {
            myIndex: 0
            title: dsTr("Theme")
        }

        DSeparatorHorizontal {}

        WindowThemeExpand {
            myIndex: 1
            title: dsTr("Window")
        }

        DSeparatorHorizontal {}

        IconThemeExpand {
            myIndex: 2
            title: dsTr("Icon")
        }

        DSeparatorHorizontal {}

        //CursorThemeExpand {
            //myIndex: 3
            //title: dsTr("Cursor")
        //}

        //DSeparatorHorizontal {}

        WallpaperExpand {
            myIndex: 4
            title: dsTr("Wallpaper")
        }

        DSeparatorHorizontal {}

        //GreeterThemeExpand{
            //myIndex: 5
            //title: dsTr("Greeter")
        //}

        //DSeparatorHorizontal {}

        FontSettingsExpand {
            myIndex: 6
            title: dsTr("Fonts")
        }

        DSeparatorHorizontal {}
    }
}
