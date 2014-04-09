import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
	id: title_separator
	width: 100
	height: 36

	property int titleSepSpacing: 15
	property int leftMargin: 15
	property int rightMargin: 15
	property string title: ""

	DssH2 {
		id: title
		text: title_separator.title
		anchors.left: parent.left
		anchors.leftMargin: title_separator.leftMargin
		anchors.verticalCenter: parent.verticalCenter
	}
	DSeparatorHorizontal {
		anchors.left: title.right
		anchors.right: parent.right
		anchors.leftMargin: title_separator.titleSepSpacing
		anchors.rightMargin: title_separator.rightMargin
		anchors.verticalCenter: parent.verticalCenter
	}
}