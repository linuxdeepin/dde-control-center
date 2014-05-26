import QtQuick 2.1
import Deepin.Widgets 1.0
import "../../shared"
import "../widgets"

Column {
    id: rootPage
    width: parent.width
    property int realHeight: childrenRect.height

    property var connectionPath: dbusNetwork.CreateConnection(nmConnectionTypePppoe, "/")
    property var connectionSession: connectionSessionBuilder.createObject(rootPage, { path: connectionPath })


    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("New PPPOE Connection")
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

        PropertyLine {
            id: dslName
            title.text: dsTr("Name")
            section: "general"
            key: "id"
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
                Component.onCompleted: {
                    text = unmarshalJSON(connectionSession.GetKey(dslName.section, dslName.key))
                }
            }

            function getValue(){
                return content.item.text
            }

            function checkKey(){
                var valid = content.item.text != ""
                if(!valid){
                    content.item.state = "warning"
                }
                return valid
            }
        }

        PropertyLine {
            id: dslUserName
            title.text: dsTr("Username")
            section: "pppoe"
            key: "username"
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
            }
            function getValue(){
                return content.item.text
            }

            function checkKey(){
                var valid = content.item.text != ""
                if(!valid){
                    content.item.state = "warning"
                }
                return valid
            }
        }

        PropertyLine {
            id: dslPassword
            title.text: dsTr("Password")
            section: "pppoe"
            key: "password"
            content.sourceComponent: DTextInput{
                activeFocusOnTab: true
                width: addDslBox.rightWidth
                echoMode: TextInput.Password
            }
            function getValue(){
                return content.item.text
            }

            function checkKey(){
                return true
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
            onClicked: {
                connectionSession.Close()
                stackView.reset()
            }
        }

        DTextButton {
            text: dsTr("Add")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(checkValid()){
                    createDslConnection()
                }
            }
        }
    }

    function checkValid(){
        var valid = true
        for(var i=0; i<addDslBox.children.length; i++){
            var objLine = addDslBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                valid = objLine.checkKey() && valid
            }
        }
        return valid
    }

    function createDslConnection(){
        for(var i=0; i<addDslBox.children.length; i++){
            var objLine = addDslBox.children[i]
            if(objLine.objectName == "PropertyLine" && objLine.visible){
                setValue(objLine.section, objLine.key, objLine.getValue())
            }
        }
        if(!connectionSession.Save()){
            print("Create dsl connection error:", connectionSession.errors)
        }
        stackView.reset()
    }

    function setValue(section, key, value){
        connectionSession.SetKey(section, key, marshalJSON(value))
    }

}
