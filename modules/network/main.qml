import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
//import DBus.Com.Deepin.Daemon.Network 1.0
import "../widgets"

Item {
    ColumnLayout {
	width: parent.width
	RowLayout {
	    anchors.left:parent.left
	    anchors.leftMargin: 10

	    Layout.preferredWidth: parent.width  - anchors.leftMargin
	    Layout.preferredHeight: 50
	    Label {
		text: dsTr("Network Settings")
		color: textColor
		font.pointSize: 12
	    }
	    DTextButton {
		text: dsTr("Create Connections")
		anchors.right: abc.left
	    }
	    DTextButton {
		anchors.right: parent.right
		id: abc
		text: "i"
		anchors.rightMargin: 5
	    }
	}


	ToggleSwitcher {
	    toggled: true

	    id: c1
	    title: dsTr("Wired Connections")

	    content: Column {
		Text { text:"4444444"}
		Text { text:"4444444"}
		Text { text:"4444444"}
		Text { text:"4444444"}
		DSepratorHorizontal {}
	    }
	}

	ToggleSwitcher {
	    toggled: true
	    anchors.top: c1.bottom

	    id: c2
	    title: dsTr("Wireless Connections")
	    content: Column {
		RowLayout {
		    Image {
			id: ch
			source: "../network/img/check_box-2.png"
		    }
		    DExpandRect {
			width: c2.width - ch.width
			head.text: "Realtek PCIe GBE Family Controller"
		    }
		}
		DSepratorHorizontal {}
	    }
	}

	ToggleSwitcher {
	    //toggled: true
	    anchors.top: c2.bottom

	    id:c3
	    title: dsTr("VPN Connections")

	    content: Column { 
		Text {text:"1234444"}
		DSepratorHorizontal {}
	    }
	}

	ToggleSwitcher {
	    //toggled: true
	    title: dsTr("DSL Connections")
	    id:c4

	    content: Column {
		Text { text:"4444444"}
		DSepratorHorizontal {}
		width:c4.width
	    }
	    anchors.top: c3.bottom
	}
    }
}
