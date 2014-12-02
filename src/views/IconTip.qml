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

import QtQuick 2.1
import Deepin.Locale 1.0
import Deepin.Widgets 1.0

DSingleLineTip {
    width: 100
    height: 25
    radius:2
    textColor:"#ffffff"
    backgroundColor:"#040404"
    arrowHeight: 9
    destroyInterval: 200
    x:100
    y:500

    property alias animationEnable: yAnimation.enabled

    Behavior on y {
        id:yAnimation
        SmoothedAnimation {duration:  300}
    }
}
