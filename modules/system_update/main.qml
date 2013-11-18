import QtQuick 2.1
import QtQuick.Controls 1.0

Item {
    function querySize(name) {
        var result = module_system_update.get_download_size(name)
        result = JSON.parse(result)
        var pkg_status = ''
        var pkg_size = result[1]
        if (result[0] == 0){
            pkg_status = 'Installed'
        }
        else{
            if (result[0] == 1){
                pkg_status = 'Unstalled'
            }
            else {
                pkg_status = 'No package for name: \"' + name + '\"'
                pkg_size = 0
            }
        }
        sizeStatus.text = 'Status: ' + pkg_status + "\n   Size: " + pkg_size
    }

    Rectangle {
        color: "#252627"
        anchors.fill: parent
        Input {
            id: searchKeywords
            width: 180
            anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20 }
        }

        Button {
            id: searchButton
            text: "search"
            height: 25
            width: 50
            anchors { top: parent.top; topMargin: 20; left: searchKeywords.right; leftMargin: 10;}
            onClicked: {querySize(searchKeywords.text)}
        }
        Text {
            id: sizeStatus
            anchors.top: searchKeywords.bottom
            anchors.topMargin: 10
            anchors.left: searchKeywords.left
            font.pixelSize: 14
            color: 'white'
            text: 'Loading...'
        }
    }
}
