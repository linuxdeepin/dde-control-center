import QtQuick 2.0
import Deepin.Widgets 1.0

DImageCheckButton{
    property url onImage: ""
    property url offImage: ""

    anchors.verticalCenter: parent.verticalCenter
    inactivatedNormalImage: offImage
    inactivatedHoverImage: inactivatedNormalImage
    inactivatedPressImage: inactivatedNormalImage

    activatedNormalImage: onImage
    activatedHoverImage: activatedNormalImage
    activatedPressImage: activatedNormalImage
}
