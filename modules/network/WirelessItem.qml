import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import "../shared/"

Item {
    id: wirelessItem
    width: parent.width
    height: childrenRect.height

    property string devicePath: "/"
    property string uuid: dbusNetwork.GetConnectionUuidByAccessPoint(apPath)
    property bool apConnected: getIndexFromArray(uuid, activeConnections) != -1

    Behavior on height {
        PropertyAnimation { duration: 100 }
    }

    function requestUuid(){
        // TODO get uuid each time to fix connection issue temporary
        var uuid = dbusNetwork.GetConnectionUuidByAccessPoint(apPath)
        print("uuid=",uuid, uuid=="")
        if(uuid == ""){
            uuid = dbusNetwork.CreateConnectionForAccessPoint(apPath)
            print("create connection, uuid=",uuid)
        }
        if(uuid != ""){
            wirelessItem.uuid = uuid
        }
        if(wirelessItem.uuid == ""){
            wirelessItem.uuid = dbusNetwork.CreateConnectionForAccessPoint(apPath)
        }
    }

    function activateThisConnection(){
        dbusNetwork.ActivateConnectionForAccessPoint(apPath, wirelessItem.devicePath)
        wirelessDevicesExpand.inConnectingApPath = apPath
    }

    function goToEditConnection(){
        wirelessItem.requestUuid()
        stackView.push({
            "item": stackViewPages["wirelessPropertiesPage"],
            "properties": { "uuid": wirelessItem.uuid, "devicePath": wirelessItem.devicePath },
            "destroyOnPop": true
        })
        stackView.currentItemId = "wirelessPropertiesPage"
    }

    DBaseLine {
        id: wirelessLine

        property bool hovered: false
        property bool selected: false

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true
            visible: !networkModule.inPasswordInputting

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                // if security in eap, go to edit connection always
                if(apSecuredInEap) {
                    print("secured in eap") // TODO debug
                    goToEditConnection()
                }
                else{
                    if(!apConnected){
                        print("activate connection") // TODO debug
                        activateThisConnection()
                    }
                    else{
                        print("edit connection") // TODO debug
                        goToEditConnection()
                    }
                }
            }
        }
        color: dconstants.contentBgColor

        leftLoader.sourceComponent: Item {
            height: parent.height
            width: childrenRect.width

            DImageButton {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                normal_image: "img/check_1.png"
                hover_image: "img/check_2.png"
                visible: {
                    if(wirelessDevicesExpand.inConnectingApPath != "/"){
                        return false
                    }
                    else{
                        return apConnected
                    }
                }
                onClicked: {
                    // TODO
                    // dbusNetwork.DeactivateConnection(uuid)
                    dbusNetwork.DeleteConnection(uuid)
                }
            }

            WaitingImage {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                on: wirelessDevicesExpand.inConnectingApPath == apPath
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                text: apName
                font.pixelSize: 12
                color: {
                    if(wirelessLine.selected){
                        return dconstants.activeColor
                    }
                    else if(wirelessLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: Row {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            Image {
                source: {
                    var power = apSignal
                    var secure = apSecured ? "-secure": ""
                    if (power <= 5)
                        return "img/ap-signal-0" + secure + ".svg"
                    else if (power <= 25)
                        return "img/ap-signal-25" + secure + ".svg"
                    else if (power <= 50)
                        return "img/ap-signal-50" + secure + ".svg"
                    else if (power <= 75)
                        return "img/ap-signal-75" + secure + ".svg"
                    else if (power <= 100)
                        return "img/ap-signal-100" + secure + ".svg"
                }
            }

            DArrowButton {
                onClicked: {
                    goToEditConnection()
                }
            }
        }
    }

    Item{
        id: passwordArea
        anchors.top: wirelessLine.bottom
        anchors.topMargin: 0 - arrowRectBackground.arrowHeight
        width: parent.width
        height: 0
        property int realHeight: childrenRect.height
        clip: true

        property string path: ""
        property string encryptionName: ""

        function showArea(encryptionName){
            passwordArea.encryptionName = encryptionName
            height = realHeight
            networkModule.inPasswordInputting = true
            passwordInput.forceActiveFocus()
        }

        function hideArea(){
            height = 0
            networkModule.inPasswordInputting = false
        }

        function cancelAction(){
            hideArea()
            dbusNetwork.CancelSecret(passwordArea.path, passwordArea.encryptionName)
        }

        function connectAction(){
            hideArea()
            dbusNetwork.FeedSecret(passwordArea.path, passwordArea.encryptionName, passwordInput.text)
        }

        Connections {
            target: networkModule
            onNeedSecretsEmit: {
                if (wirelessItem.uuid != ""){
                    passwordArea.path = dbusNetwork.GetConnectionByUuid(wirelessItem.uuid)
                    print(passwordArea.path)
                    if(passwordArea.path == path){
                        passwordArea.showArea(encryptionName)
                    }
                }
            }
        }

        ArrowRect{
            id: arrowRectBackground
            anchors.fill: parent
            fillStyle: dconstants.bgColor
            stroke: false
            radius: 0
            lineWidth: 0
            arrowPosition: 0.15
        }

        Row{
            id: passwordInputArea
            anchors.top: parent.top
            anchors.topMargin: arrowRectBackground.arrowHeight + 18
            anchors.left: parent.left
            anchors.leftMargin: 30
            height: childrenRect.height
            spacing: 10

            DssH2 {
                text: dsTr("Password: ")
            }

            Column{
                DTextInput{
                    id: passwordInput
                    textInput.color: dconstants.fgColor
                    echoMode: showPasswordButton.checked ? TextInput.Normal : TextInput.Password
                    onAccepted: {
                        passwordArea.connectAction()
                    }
                }
                Row{
                    spacing: 6

                    DSwitchButton{
                        id: showPasswordButton
                    }

                    DssH3{
                        text: dsTr("Show password")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Row{
            anchors.top: passwordInputArea.bottom
            anchors.right: parent.right
            anchors.rightMargin: 15
            height: childrenRect.height
            spacing: 10

            DTextButton{
                text: dsTr("Cancel")
                onClicked: {
                    passwordArea.cancelAction()
                }
            }

            DTextButton{
                text: dsTr("Connect")
                onClicked: {
                    passwordArea.connectAction()
                }
            }
        }
    }

    DSeparatorHorizontal{
        anchors.top: wirelessLine.bottom
        anchors.topMargin: passwordArea.height == 0 ? 0 : passwordArea.height - arrowRectBackground.arrowHeight
        visible: index != parent.ListView.view.count - 1
    }
}
