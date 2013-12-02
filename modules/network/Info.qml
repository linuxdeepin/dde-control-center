import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import DBus.Com.Deepin.Daemon.Network 1.0
import "../widgets"


Item {
    ColumnLayout{
	height: 34
	width: parent.width
	Column{
	    width: root.width
	    height: 30
	    DSepratorHorizontal{}
	    Label {
		height: 30
		anchors.leftMargin: 10
		anchors.left: parent.left
		text: "当前活动的网络链接"
		color: fgColor
	    }
	    DSepratorHorizontal{}
	}
	DExpandRect {
	    title: Text { text:"Realtek PCIe GBE Family Controller" }
	    width: root.width
	}
	GridLayout {
	    width: root.width
	    columns: 2
	    Layout.preferredWidth: root.width
	    Label { text: "接口"} Label {text: "以太网"}
	    Label { text: "硬件地址"} Label {text: "c8:60:00:de:79:f6"}
	    Label { text: "驱动"} Label {text: "r8619" }
	    Label { text: "速度"} Label {text: "100Mb/s" }
	}

    }
    Component.onCompleted: {
	console.log(nm.aPs)
    }
}
