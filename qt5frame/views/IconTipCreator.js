/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Wanqing Yang
**
**  Author:     Wanqing Yang <match.yangwanqing@gmail.com>
**  Maintainer: Wanqing Yang <match.yangwanqing@gmail.com>
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

var iconTipComponent = Qt.createComponent("IconTip.qml");
var iconTipPage;
var pageX = 0
var pageY = 0
var pageWidth = 0
var toolTip = ""


function showIconTip()
{
    if (iconTipPage != undefined)
    {
        iconTipPage.x = pageX
        iconTipPage.y = pageY
        iconTipPage.width = pageWidth;
        iconTipPage.toolTip = toolTip
        iconTipPage.showTipAtRight()
        return
    }
    else
    {
        var iconTipComponent = Qt.createComponent("IconTip.qml")
        if (iconTipComponent.status === Component.Ready)
            fnishCreate();
        else if (iconTipComponent.status === Component.Error)
        {
            console.log("component Error:", iconTipComponent.errorString());
        }
        else{
            iconTipComponent.statusChanged.connect(fnishCreate);
        }
    }
}

function fnishCreate()
{
    if (iconTipComponent.status === Component.Ready)
    {
        iconTipPage = iconTipComponent.createObject(undefined);

        if (iconTipPage === null)
        {
            console.log("component createObject failed");
        }
        else
        {
            iconTipPage.x = pageX
            iconTipPage.y = pageY
            iconTipPage.width = pageWidth;
            iconTipPage.toolTip = toolTip
            iconTipPage.showTipAtRight();
        }
    }
    else if (iconTipComponent.status === Component.Error) {
        console.log("Error loading component:", iconTipComponent.errorString());
    }
}


function destroyiconTip()
{
    if (iconTipPage != undefined)
        iconTipPage.destroyTip()
}

function setAnimationEnable(flag)
{
    if (iconTipPage != undefined)
        iconTipPage.animationEnable = flag
}
