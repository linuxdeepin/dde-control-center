import QtQuick 2.0
import Deepin.Widgets 1.0

DImageCheckButton{
    property url onImage: ""
    property url offImage: ""

    anchors.verticalCenter: parent.verticalCenter
    inactivatedNomralImage: offImage
    inactivatedHoverImage: inactivatedNomralImage
    inactivatedPressImage: inactivatedNomralImage

    activatedNomralImage: onImage
    activatedHoverImage: activatedNomralImage
    activatedPressImage: activatedNomralImage
}
