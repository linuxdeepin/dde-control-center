import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column{
    width: parent.width

    property var accessPoint

    DBaseLine{
        height: 38
        leftLoader.sourceComponent: DssH2{
            text: "Access Point :  " + accessPoint[0]
        }
    }

    DSeparatorHorizontal {}

}
