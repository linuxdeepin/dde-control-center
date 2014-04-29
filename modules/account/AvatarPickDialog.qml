import QtQuick 2.1
import QtQuick.Dialogs 1.0

FileDialog {
    title: dsTr("Choose a new picture for your account")
    selectExisting: true
    selectFolder: false
    selectMultiple: false
    nameFilters: [ "Image files (*.jpg *.png *.jpeg)" ]
}
