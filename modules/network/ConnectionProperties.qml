import QtQuick 2.1
import Deepin.Widgets 1.0
// import "components_section"
import "components_section_autogen"

BaseConnectionProperties {
    DSeparatorHorizontal {}
    DBaseLine {
        rightLoader.sourceComponent: Row {
            spacing: 6

            DTextButton {
                text: dsTr("Save")
                onClicked: {
                    checkAllKeys()
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
