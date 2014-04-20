import QtQuick 2.1
import Deepin.Widgets 1.0
import "components"
// import "components_autogen"

BaseEditPage {
    id: root
    activeExpandIndex: 0

    EditTitle {}

    DSeparatorHorizontal {}
    EditSectionGeneral {
        myIndex: 0
        activeExpandIndex: root.activeExpandIndex
    }

    DSeparatorHorizontal {}
    EditSectionIpv4 {
        myIndex: 1
        activeExpandIndex: root.activeExpandIndex
    }

    DSeparatorHorizontal {}
    EditSectionSecurity {
        myIndex: 2
        activeExpandIndex: root.activeExpandIndex
    }

    DSeparatorHorizontal {}
    DBaseLine {
        rightLoader.sourceComponent: Row {
            spacing: 6

            DTextButton {
                text: dsTr("Save")
                onClicked: {
                    if (connectionSessionObject.Save()) {
                        stackView.reset()
                    }
                }
            }

            DTextButton {
                text: dsTr("Close")
                onClicked: {
                    connectionSessionObject.Close()
                    stackView.reset()
                }
            }
        }
    }

}
