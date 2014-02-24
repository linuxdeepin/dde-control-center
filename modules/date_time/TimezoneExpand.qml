import QtQuick 2.1
import Deepin.Widgets 1.0

Column {
    id: timezoneArea
    width: parent.width
    height: childrenRect.height

    property string currentActionStateName: ""
    property var listModelComponent: DListModelComponent {}

    property var userTimezoneList: ["Asia/Shanghai"]

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("Timezone")
        }

        rightLoader.sourceComponent: StateButtons {
            onCurrentActionStateNameChanged: {
                timezoneArea.currentActionStateName = currentActionStateName
            }
        }

    }

    DSeparatorHorizontal {}

    Rectangle {
        id: userTimezoneListArea
        width: parent.width
        visible: timezoneArea.currentActionStateName != "addButton"

        ListView {
            width: parent.width
            height: model.count * 28

            property string currentSelectedTimezone: gDate.currentTimezone
            model: ListModel {}
        }
    }
}
