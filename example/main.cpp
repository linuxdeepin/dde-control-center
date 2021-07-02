#include "networkhandler.h"

#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator t;
    if (t.load(":/qm/network_cn_qm"))
        app.installTranslator(&t);

    NetworkHandler h;
    app.installEventFilter(&h);

    return app.exec();
}
