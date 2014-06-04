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
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Slider {
    id: slider
    width: 196
    property int grooveWidth: width - handleWidth + 2
    property int grooveHeight: 8
    property int handleWidth: 18

    property int _buttonWidth: 0

    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: grooveWidth + handleWidth - 2
            implicitHeight: grooveHeight * 2
            clip: true
            color: Qt.rgba(1, 1, 1, 0)

            Rectangle{
                implicitWidth: grooveWidth
                implicitHeight: grooveHeight
                clip: true
                radius: implicitHeight
                anchors.centerIn: parent
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "black" }
                    GradientStop { position: 1.0; color: "#303132" }
                }

                Rectangle {
                    implicitWidth: parent.width - 2
                    implicitHeight: grooveHeight - 2
                    anchors.centerIn: parent
                    radius: implicitHeight
                    color: Qt.rgba(15/255, 15/255, 15/255, 1.0)

                    Rectangle {
                        width: parent.width * (slider.value - slider.minimumValue)/(slider.maximumValue - slider.minimumValue)
                        height: parent.height
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        radius: height
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Qt.rgba(0, 104/255, 170/255, 1.0) }
                            GradientStop { position: 1.0; color: Qt.rgba(91/255, 164/255, 211/255, 1.0) }
                        }
                    }
                }
            }


        }
        handle: Image {
            source: "images/slider_handle.png"
            Component.onCompleted: {
                _buttonWidth = width
            }
        }
    }

}
