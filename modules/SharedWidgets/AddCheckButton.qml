import QtQuick 2.1
import Deepin.Widgets 1.0

DImageCheckButton {
    inactivatedNomralImage: "images/add_normal.png"
    inactivatedHoverImage: "images/add_hover.png"
    inactivatedPressImage: "images/add_press.png"

    activatedNomralImage: inactivatedPressImage
    activatedHoverImage: inactivatedPressImage
    activatedPressImage: inactivatedPressImage
}
