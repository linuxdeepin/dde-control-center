import QtQuick 2.1
import DBus.Org.Freedesktop.UDisks2 1.0

Rectangle {
	
	ObjectManager {
		id: objManager
	}
	
	Component.onCompleted: {
		print(udisk2.getManagedObjects())
	}
}
