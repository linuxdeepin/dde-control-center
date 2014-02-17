import QtQuick 2.1
import Deepin.Widgets 1.0

DImageCheckButton {
    inactivatedNomralImage: "images/delete_normal.png"
    inactivatedHoverImage: "images/delete_hover.png"
    inactivatedPressImage: "images/delete_press.png"

    activatedNomralImage: inactivatedPressImage
    activatedHoverImage: inactivatedPressImage
    activatedPressImage: inactivatedPressImage
}
