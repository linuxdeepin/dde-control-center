import QtQuick 2.1

GridView {
    id: theme_view

    width: 310
    height: 400

    cellWidth: width / 3
    cellHeight: height / 3
    
    property int horizontalSpacing: 5
    property int verticalSpacing: 10
    
    signal itemSelected (int idx, string themeId)
    signal selectItemPrivate (string id)
    
    function select(themeId) {
        selectItemPrivate(themeId)
    }

    model: ListModel {
        ListElement {
            themeId: "theme0"
        }
        ListElement {
            themeId: "theme1"
        }
        ListElement {
            themeId: "theme2"
        }
        ListElement {
            themeId: "theme3"
        }
        ListElement {
            themeId: "theme4"
        }
        ListElement {
            themeId: "theme5"
        }
        ListElement {
            themeId: "theme6"
        }
    }
    delegate: Component {
        Item{
            id: delegate

            width: cellWidth
            height: cellHeight
            property real contentWidth: (GridView.view.width - horizontalSpacing * 2) / 3
            property real contentHeight: (GridView.view.height - verticalSpacing * 2) / 3
            
            function shouldHorizCenter(index) {
                return !(index % 3 == 0 || (index + 1) % 3 == 0)
            }
            
            function shouldVertiCenter(index) {
                return !(index < 3 || Math.floor(index / 3) + 1 == Math.ceil(delegate.GridView.view.count / 3) )
            }
            
            states: [
                State {
                    name: "normal"
                    PropertyChanges {
                        target: theme_item
                        state: "normal"
                    }
                },
                State {
                    name: "selected"
                    PropertyChanges {
                        target: theme_item
                        state: "selected"
                    }
                }
            ]
            
            Connections {
                target: delegate.GridView.view
                onSelectItemPrivate: {
                    if (id == themeId) {
                        delegate.state = "selected"
                        delegate.GridView.view.itemSelected(index, themeId)
                    } else {
                        delegate.state = "normal"
                    }
                }
            }
            
            Item {
                width: parent.contentWidth
                height: parent.contentHeight
                
                ThemeItem {
                    id: theme_item
                    anchors.fill: parent
                    
                    onSelected: {
                        delegate.GridView.view.select(themeId)
                    }
                }
                
                Binding on anchors.left {
                    when: index % 3 == 0
                    value: delegate.left
                }
                Binding on anchors.right {
                    when: (index + 1) % 3 == 0
                    value: delegate.right
                }
                Binding on anchors.top {
                    when: index < 3
                    value: delegate.top
                }
                Binding on anchors.bottom {
                    when: Math.floor(index / 3) + 1 == Math.ceil(delegate.GridView.view.count / 3) 
                    value: delegate.bottom
                }
                Binding on anchors.verticalCenter {
                    when: delegate.shouldVertiCenter(index)
                    value: delegate.verticalCenter
                }
                Binding on anchors.horizontalCenter {
                    when: delegate.shouldHorizCenter(index)
                    value: delegate.horizontalCenter
                }
            }
        }
    }
}