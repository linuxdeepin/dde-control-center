import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../widgets"

DColumn {
    id:body
    width: root.width

    DBaseExpand {
	id:c3
	header: DRightArrowHeader {
	    text: dsTr("VPN Connections")
	    onToggled: c3.expanded = !c3.expanded
	}

	content: Column { 
	    Text {text:"1234444"}
	}
    }

    DBaseExpand {
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
