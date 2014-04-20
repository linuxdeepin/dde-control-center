import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    height: 38
    leftLoader.sourceComponent: Row{
        spacing: 6
        DssH2{
            text: dsTr("Connection Name:")
        }
        DTextInput{
            textInput.color: dconstants.fgColor
            // TODO page name
            text: generalGetKey("General", "id")
        }
    }
}
