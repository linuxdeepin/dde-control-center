import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditLine {
    id: root
    
    // TODO
    property int min
    property int max
    
    rightLoader.sourceComponent: DSpinner {
        // Binding on active {
        //     when: root.value != undefined
        //     value: root.value
        // }
        // onActiveChanged: {
        //     root.value = active
        //     setKey()
        // }
    }
}
