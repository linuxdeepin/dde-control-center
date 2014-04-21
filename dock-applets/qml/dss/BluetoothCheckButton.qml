import QtQuick 2.0
import Deepin.Widgets 1.0

DImageCheckButton{
    anchors.verticalCenter: parent.verticalCenter
    inactivatedNomralImage: "images/bluetooth_off.png"
    inactivatedHoverImage: inactivatedNomralImage
    inactivatedPressImage: inactivatedNomralImage

    activatedNomralImage: "images/bluetooth_on.png"
    activatedHoverImage: activatedNomralImage
    activatedPressImage: activatedNomralImage
}
