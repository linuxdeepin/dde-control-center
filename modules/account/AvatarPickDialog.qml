import QtQuick 2.1
import QtQuick.Dialogs 1.0

FileDialog {
    title: dsTr("Please choose one image as you avatar")
    selectExisting: true
    selectFolder: false
    selectMultiple: false
    nameFilters: [ "Image files (*.jpg *.png *.jpeg)" ]
}
