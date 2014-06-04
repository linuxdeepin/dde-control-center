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

Rectangle {
    id: imageCheckButton

    property string inactivatedNomralImage
    property string inactivatedHoverImage
    property string inactivatedPressImage

    property string activatedNomralImage
    property string activatedHoverImage
    property string activatedPressImage

    property bool activated: false
    property bool selected: false

    property bool hover: false

    signal clicked

    onSelectedChanged: {
        if (selected && hover){
            currentImage.source = activatedHoverImage
            activated = true
        }
        else if (selected && !hover) {
            currentImage.source = activatedNomralImage
            activated = true
        }
        else if (!selected && hover) {
            currentImage.source = inactivatedHoverImage
            activated = false
        }
        else if (!selected && !hover) {
            currentImage.source = inactivatedNomralImage
            activated = false
        }
    }

    Image {
        id: currentImage
        anchors.centerIn: parent
        source: inactivatedNomralImage
        scale: 0.875
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            imageCheckButton.clicked() 
            if (!imageCheckButton.selected) {
                imageCheckButton.activated = !imageCheckButton.activated
            }
        }

        onPressed: {
            if (imageCheckButton.activated) {
                currentImage.source = activatedPressImage
            }
            else {
                currentImage.source = inactivatedPressImage
            }
        }

        onReleased: {

            if (imageCheckButton.activated) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onEntered: {
            imageCheckButton.hover = true
            if (imageCheckButton.activated) {
                currentImage.source = activatedHoverImage
            }
            else {
                currentImage.source = inactivatedHoverImage
            }
        }

        onExited: {
            imageCheckButton.hover = false
            if (imageCheckButton.activated) {
                currentImage.source = activatedNomralImage
            }
            else {
                currentImage.source = inactivatedNomralImage
            }
        }
    }
}
