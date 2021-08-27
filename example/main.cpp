#include "networkhandler.h"

#include <QApplication>
#include <QDesktopWidget>

#include "dccplugintestwidget.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator t;
    if (t.load(":/qm/network_cn_qm"))
        app.installTranslator(&t);

    if (QString(argv[1]) == "dccPlug") {
        DccPluginTestWidget testPluginWidget;
        testPluginWidget.setFixedSize(1024, 720);
        QDesktopWidget *deskdop = QApplication::desktop();
        testPluginWidget.move((deskdop->width() - testPluginWidget.width()) / 2, (deskdop->height() - testPluginWidget.height()) / 2);

        testPluginWidget.show();
        return app.exec();
    }

    NetworkHandler h;
    app.installEventFilter(&h);

    return app.exec();
}
