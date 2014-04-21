import QtQuick 2.0
import Deepin.Widgets 1.0

DImageCheckButton{
    anchors.verticalCenter: parent.verticalCenter
    inactivatedNomralImage: "images/vpn_off.png"
    inactivatedHoverImage: inactivatedNomralImage
    inactivatedPressImage: inactivatedNomralImage

    activatedNomralImage: "images/vpn_on.png"
    activatedHoverImage: activatedNomralImage
    activatedPressImage: activatedNomralImage
}
