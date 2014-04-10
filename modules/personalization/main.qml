import QtQuick 2.1
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Themes 1.0
import "components"
import "widgets"

Item {
    id: personalizationModule
    anchors.fill: parent

    property int cellWidth: 144
    property int cellHeight: 112

    property var constants: DConstants {}
    property var listModelComponent: DListModelComponent {}

    property int activeExpandIndex: 0

    property var dbusThemeManager: ThemeManager{}
    property var dbusThumbPath: ThumbPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var dbusPreviewPath: PreviewPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var currentThemeObject: {
        var path = dbusThemeManager.GetPathViaName(dbusThemeManager.currentTheme)
        var obj = themeComponent.createObject(personalizationModule, { path: path[0] })
        return obj
    }

    property var previewsWindow: PreviewWindow{
        x: rootWindow.x - width
        y: rootWindow.y + 18
        themeObject: currentThemeObject
    }

    Component{
        id: themeComponent
        Theme {}
    }

    Column {
        id: contentArea
        width: parent.width
        height: parent.height

        property int maxConetentHeight: height - personalizationTitle.height -  32 * 6 - 2

        DssTitle {
            id: personalizationTitle
            text: dsTr("Personalization")
        }

        DSeparatorHorizontal {}

        ThemeExpand {}

        DSeparatorHorizontal {}

        WindowThemeExpand {}

        DSeparatorHorizontal {}

        CursorThemeExpand {}

        DSeparatorHorizontal {}
        
        WallpapperExpand {}

        DSeparatorHorizontal {}

        IconThemeExpand {}

        DSeparatorHorizontal {}

        FontSettingsExpand {}

        DSeparatorHorizontal {}
    }
}
