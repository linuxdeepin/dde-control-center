import QtQuick 2.0

Rectangle {
    width: 200; height: 200

    Column {
        spacing: 10
        anchors.top: parent.top
        Repeater{
            id: itemList
            model: 3
            delegate: Loader {
                id: loader
                focus: false
                property int itemIndex: index
                sourceComponent: keyReader
            }
        }
    }

    Component {
        id: keyReader 
        Item {
            width: 200
            height: 30
            property alias realField: field

            Rectangle {
                id: field
                anchors.fill: parent
                focus: false
                color: !activeFocus ? "red" : "white"
                Keys.onPressed: {
                    console.log("Loaded item captured from ", itemIndex);
                    event.accepted = true;
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        parent.focus = true
                        for(var i=0;i<itemList.count;i++){
                            if (itemList.itemAt(i).itemIndex != itemIndex){
                                itemList.itemAt(i).focus = false
                                itemList.itemAt(i).item.realField.focus = false
                            }
                            else{
                                itemList.itemAt(i).focus = true
                                itemList.itemAt(i).item.realField.focus = true
                            }
                        }
                    }
                }
            }
        }

    }
}
