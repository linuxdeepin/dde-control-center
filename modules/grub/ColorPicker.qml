import QtQuick 2.1

Item {
    id: root
    width: 300
    height: 300
    property var colors: [["#980000", "#FF0000", "#FF9900", "#FFFF00", "#00FF00",
                           "#00FFFF", "#4A86E8", "#0000FF", "#9900FF", "#FF00FF"],
                          ["#E6B8AF", "#F4CCCC", "#FCE5CD", "#FFF2CC", "#D9EAD3",
                           "#D0E0E3", "#C9DAF8", "#CFE2F3", "#D9D2E9", "#EAD1DC"],
                          ["#DD7E6B", "#EA9999", "#F9CB9C", "#FFE599", "#B6D7A8",
                           "#A2C4C9", "#A4C2F4", "#9FC5E8", "#B4A7D6", "#D5A6BD"],
                          ["#CC4125", "#E06666", "#F6B26B", "#FFD966", "#93C47D",
                           "#76A5AF", "#6D9EEB", "#6FA8DC", "#8E7CC3", "#C27BA0"],
                          ["#A61C00", "#CC0000", "#E69138", "#F1C232", "#6AA84F",
                           "#45818E", "#3C78D8", "#3D85C6", "#674EA7", "#A64D79"],
                          ["#85200C", "#990000", "#B45F06", "#BF9000", "#38761D",
                           "#134F5C", "#1155CC", "#0B5394", "#351C75", "#741B47"],
                          ["#5B0F00", "#660000", "#783F04", "#7F6000", "#274E13",
                           "#0C343D", "#1C4587", "#073763", "#20124D", "#4C1130"]]
    
    signal colorSet (color clr)
    signal selectColorPrivate (color clr)
    
    function selectColor(clr) {
        selectColorPrivate(clr)
    }

    GridView {
        id: gridview
        anchors.fill: parent
        model: 70
        cellWidth: width / 10
        cellHeight: cellWidth
        delegate: Item {
            width: gridview.cellWidth
            height: gridview.cellHeight
            Rectangle {
                width: 25
                height: 25
                color: root.colors[Math.floor(index / 10)][index % 10]
                anchors.centerIn: parent
                
                Connections {
                    target: root
                    onSelectColorPrivate: {
                        if (clr == parent.color) {
                            // Select operation here
                        }
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.colorSet(parent.color)
                    }
                }
            }
        }
    }
}