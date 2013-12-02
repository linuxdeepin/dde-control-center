import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

DColumn {
    id:body
    width: root.width
    DExpandRect {
	id: c1
	header: DToggleButtonHeader {
	    text: dsTr("Wired Connections")
	    onToggled: c1.toggle()
	}
	content: Column {
	    Text { text:"4444444"}
	    Text { text:"4444444"}
	    Text { text:"4444444"}
	    Text { text:"4444444"}
	}
    }

    DExpandRect {
	id: c2
	header: DToggleButtonHeader {
	    text: dsTr("Wireless Connections")
	    onToggled: c2.toggle()
	}
	content: Column {
	    RowLayout {
		Image {
		    id: ch
		    source: "../network/img/check_box-2.png"
		}
	    }
	}
    }

    DExpandRect {
	id:c3
	header: DRightArrowHeader {
	    text: dsTr("VPN Connections")
	    onToggled: c3.toggle()
	}

	content: Column { 
	    Text {text:"1234444"}
	}
    }

    DExpandRect {
	id:c4
	header: DDownArrowHeader {
	    text: dsTr("DSL Connections")
	    onToggled: c4.toggle()
	}

	content: Column {
	    Text {text:"4444444"}
	    DSepratorHorizontal {}
	}
    }
}
