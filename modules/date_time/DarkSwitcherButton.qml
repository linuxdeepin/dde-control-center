import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

CheckBox {
    style: CheckBoxStyle {
        indicator: Image {
            source: control.checked ? "images/switch-dark-on.svg" : "images/switch-dark-off.svg"
        }
    }
}
