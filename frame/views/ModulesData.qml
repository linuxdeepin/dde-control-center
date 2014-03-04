import QtQuick 2.1

Item {
    property var moduleLocaleNames: {
            "date_time": dsTr("Date and Time"),
            "network": dsTr("Network"),
            "disk_mount": dsTr("Disk Mount"),
            "bluetooth": dsTr("Bluetooth"),
            "sound": dsTr("Sound"),
            "power": dsTr("Power"),
            "dss": dsTr("More Settings"),
            "shutdown": dsTr("Shutdown"),
            "notification": dsTr("Notification"),
            "display": dsTr("Display"),
            "desktop": dsTr("Desktop"),
            "personalization": dsTr("Personalization"),
            "shortcuts": dsTr("Keyboard Shortcuts"),
            "keyboard": dsTr("Keyboard"),
            "mouse_touchpad": dsTr("Mouse and Touchpad"),
            "mouse": dsTr("Mouse"),
            "account": dsTr("User Accounts"),
            "default_applications": dsTr("Default Applications"),
            "system_info": dsTr("System Information"),
            "grub": dsTr("Startup Screen"),
            "home": dsTr("Home")
        }

    property var allIds: [
            "account",
            "display",
            "default_applications",
            "personalization",
            "network",
            "bluetooth",
            "sound",
            "date_time",
            "power",
            "mouse_touchpad",
            "keyboard",
            "shortcuts",
            "grub",
            "system_info"
        ]
}
