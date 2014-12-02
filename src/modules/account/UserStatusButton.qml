import QtQuick 2.1

Image {
    id: user_status_button
    state: "otherUser"
    
    signal changeStatus (bool locked)

    property string currentUserSource: "images/current_user.png"
    property string otherUserSource: "images/other_user.png"
    property string inactiveUserSource: "images/inactive_user.png"

    states: [
        State {
            name: "currentUser"
            PropertyChanges { target: user_status_button; source: currentUserSource}
        },
        State {
            name: "otherUser"
            PropertyChanges { target: user_status_button; source: otherUserSource}
        },
        State {
            name: "inactiveUser"
            PropertyChanges { target: user_status_button; source: inactiveUserSource}
        }
    ]

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (user_status_button.state == "otherUser") {
                user_status_button.state = "inactiveUser"
                user_status_button.changeStatus(true)
            } else if (user_status_button.state == "inactiveUser") {
                user_status_button.state = "otherUser"
                user_status_button.changeStatus(false)                
            }
        }
    }
}
