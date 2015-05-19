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
import QtGraphicalEffects 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Accounts 1.0

Rectangle{
    id: component_bg
    width: 310
    height: visible ? contentColumn.height : 0
    color: "#1A1B1B"

    property int leftPadding: 15
    property int nameLeftPadding: 120

    property var creatingUserInfo: null
    property string currentUserName: ""
    property string currentUserAvatar: ""
    property int currentUserType: 0

    function setCreatingUser(userInfo){
        creatingUserInfo = userInfo
        currentUserName = userInfo.userName
        currentUserAvatar = userInfo.userIconFile
        currentUserType = userInfo.userAccountType
    }

    Column{
        id: contentColumn
        width: parent.width

        Item {
            id: component_top
            width: parent.width
            height: 100

            DRoundImage {
                id: round_image
                roundRadius: 30
                imageSource: currentUserAvatar ? "file://" + currentUserAvatar : ""

                anchors.left: parent.left
                anchors.leftMargin: leftPadding + 40 - roundRadius
                anchors.verticalCenter: parent.verticalCenter
            }

            Desaturate {
                anchors.fill: round_image
                source: round_image
                desaturation: 1.0
            }

            Column {
                id: name_column
                spacing: 2

                DssH2 {
                    text: currentUserName
                    color: DConstants.fgColor
                    width: 140
                    elide: Text.ElideRight
                    font.pixelSize: 14
                }

                DssH3 {
                    text: currentUserType == 0 ? dsTr("Normal User") : dsTr("Administrator")
                    color: DConstants.fgColor
                    font.pixelSize: 10
                }

                anchors.left: parent.left
                anchors.leftMargin: nameLeftPadding
                anchors.verticalCenter: parent.verticalCenter
            }

            DssH2 {
                text: dsTr("Account Preparing...")
                color: DConstants.tuhaoColor
                font.pixelSize: 12
                anchors.left:name_column.left
                anchors.top:name_column.bottom
            }
        }

        DSeparatorHorizontal {}
    }
}
