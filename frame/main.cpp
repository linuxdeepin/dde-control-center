#include <QApplication>
#include <QFile>
#include <QDebug>

#include "frame.h"
#include "interfaces.h"
#include "logmanager.h"

#include <libdui/dapplication.h>
#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

QString getQssFromFile(const QString &name)
{
    QString qss = "";

    QFile f(name);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)){
        qss = f.readAll();
        f.close();
    }

    return qss;
}

void onThemeChange(const QString &theme)
{
    QString fileName = QString(":/resources/qss/%1.qss").arg(theme);

    qApp->setStyleSheet(getQssFromFile(fileName));
}

int main(int argv, char *args[])
{
    DApplication app(argv, args);

    Frame frame;
    frame.show();

    DThemeManager *manager = DThemeManager::instance();
    QObject::connect(manager, &DThemeManager::themeChanged, [=](QString theme){
        onThemeChange(theme);
    });

    manager->setTheme("dark");
    LogManager::instance()->debug_log_console_on();
    return app.exec();
}
