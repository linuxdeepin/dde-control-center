import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQml.Models 2.1

Item {
    height: 38
    width: 160

    property bool handHide: false

    NumberAnimation {
        id: showSearchEntyArea
        target: searchEntryArea
        properties: "width"
        to: 160
        duration: 400

        onStarted: {
            searchInput.forceActiveFocus()
            searchButtonArea.visible = false
        }
    }

    NumberAnimation {
        id: hideSearchEntyArea
        target: searchEntryArea
        property: "width"
        to: 0
        duration: 400

        onStarted: {
            handHide = true
        }

        onStopped: {
            searchButtonArea.visible = true
            if(!searchButtonMouseArea.containsMouse) {
                handHide = false
            }
        }
    }

    Item {
        id: searchEntryArea
        width: 0
        height: parent.height
        clip: true
        anchors.right: parent.right

        DTextInput {
            id: searchInput
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: ""
            onTextChanged: {
                searchResultListView.keyword = text
            }

            Keys.onEscapePressed: {
                text = ""
                if(!hideSearchEntyArea.running){
                    hideSearchEntyArea.start()
                }
            }
        }

        DOpacityImageButton {
            id: clearButton
            anchors.right: parent.right
            anchors.rightMargin: 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 1
            source: "images/clear.png"

            onClicked: {
                if(searchInput.text == "" && !hideSearchEntyArea.running){
                    hideSearchEntyArea.start()
                }
                else{
                    searchInput.text = ""
                }
            }
        }
    }

    Image {
        id: searchButtonArea
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        source: "images/search_normal.png"

        MouseArea {
            id: searchButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                if(!showSearchEntyArea.running && !handHide){
                    showSearchEntyArea.start()
                }
            }
            onExited: {
                handHide = false
            }
        }
    }

}
