import QtQuick 2.1

Image {
    id: user_status_button
    
    property string currentUserSource: "images/current_user.png"
    property string otherUserSource: "images/other_user.png"
    property string inactiveUserSource: "images/inactive_user.png"
    
    source: otherUserSource
    
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
}
