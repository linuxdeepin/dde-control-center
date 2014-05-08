import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    width: parent.width
    property int realHeight: childrenRect.height

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("新建PPPOE连接")
        }
    }

    DSeparatorHorizontal {}

    Column {
        id: addDslBox
        width: parent.width

        property int edgePadding: 24
        property int leftWidth: edgePadding
        property int rightWidth: addDslBox.width - leftWidth - edgePadding

        function updateLeftWidth(newWidth){
            if(newWidth + edgePadding > addDslBox.leftWidth){
                addDslBox.leftWidth = newWidth + edgePadding
            }
        }

        DCenterLine {
            id: dslName
            title.text: dsTr("Name")
            leftWidth: addDslBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
            }
            Component.onCompleted: {
                addDslBox.updateLeftWidth(title.contentWidth)
            }
        }

        DCenterLine {
            id: dslUserName
            title.text: dsTr("Account")
            leftWidth: addDslBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
            }
            Component.onCompleted: {
                addDslBox.updateLeftWidth(title.contentWidth)
            }
        }

        DCenterLine {
            id: dslPassword
            title.text: dsTr("Password")
            leftWidth: addDslBox.leftWidth
            color: dconstants.contentBgColor
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
            }
            Component.onCompleted: {
                addDslBox.updateLeftWidth(title.contentWidth)
            }
        }
    }

    DSeparatorHorizontal {}

    Row {
        height: 38
        spacing: 10
        anchors.right: parent.right
        anchors.rightMargin: 15

        DTextButton {
            text: dsTr("Cancel")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: stackView.reset()
        }

        DTextButton {
            text: dsTr("Finish")
            anchors.verticalCenter: parent.verticalCenter
        }
    }

}
