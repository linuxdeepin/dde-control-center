import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    width: 200
    height: 500

    color: "black"
    DSelectView {
        width: 200
        radioMode: false

        model: ListModel {
            ListElement {
                itemId: "1"
                itemText: "one"
                itemWidth: 0
            }
            ListElement {
                itemId: "2"
                itemText: "two"
                itemWidth: 0
            }
            ListElement {
                itemId: "3"
                itemText: "hello world"
                itemWidth: 0
            }
            ListElement {
                itemId: "4"
                itemText: "linux deepin"
                itemWidth: 0
            }
            ListElement {
                itemId: "5"
                itemText: "five five five"
                itemWidth: 0
            }
        }
    }
}
