import QtQuick 2.1
import Deepin.Widgets 1.0
import "components"
// import "components_autogen"

BaseEditPage {
    id: root
    activeExpandIndex: 0

    EditTitle {}

    EditSectionSeparator {relatedSection: sectionGeneral}
    EditSectionGeneral {
        myIndex: 0
        id: sectionGeneral
        activeExpandIndex: root.activeExpandIndex
    }

    EditSectionSeparator {relatedSection: sectionIpv4}
    EditSectionIpv4 {
        myIndex: 1
        id: sectionIpv4
        activeExpandIndex: root.activeExpandIndex
    }

    EditSectionSeparator {relatedSection: sectionSecurity}
    EditSectionSecurity {
        myIndex: 2
        id: sectionSecurity
        activeExpandIndex: root.activeExpandIndex
    }

    DSeparatorHorizontal {}
    DBaseLine {
        rightLoader.sourceComponent: Row {
            spacing: 6

            DTextButton {
                text: dsTr("Save")
                onClicked: {
                    saveAllKeys()
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
