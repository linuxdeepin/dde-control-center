#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QTranslator>
#include <QCommandLineParser>

#include "frame.h"
#include "interfaces.h"
#include "logmanager.h"
#include "dbus/dbuscontrolcenter.h"
#include "dbus/dbuscontrolcenterservice.h"

#include <libdui/dapplication.h>
#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

QString getQssFromFile(const QString &name)
{
    QString qss = "";

    QFile f(name);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
    app.setOrganizationName("deepin");
    app.setApplicationName("DDE Control Center");
    app.setApplicationVersion("3.0");

#ifndef QT_DEBUG
    QDir::setCurrent(QApplication::applicationDirPath());
#endif

    // install translators
    QTranslator translator;
    translator.load("../share/dde-control-center/translations/dde-control-center_" + QLocale::system().name());
    app.installTranslator(&translator);

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addPositionalArgument("module", "the module's id of which to be shown.");
    parser.process(app);

    QStringList positionalArgs = parser.positionalArguments();

    // initialize logging
    LogManager::instance()->debug_log_console_on();

    Frame frame;
    DBusControlCenterService adaptor(&frame);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
        !conn.registerObject("/com/deepin/dde/ControlCenter", &frame))
    {
        qDebug() << "dbus service already registered!";

        // call exist service
        DBusControlCenter c;
        if (!positionalArgs.isEmpty()) {
            c.ShowModule(positionalArgs.at(0));
        } else if (parser.isSet(showOption)) {
            c.Show();
        }

#ifndef QT_DEBUG
        return 0;
#endif
    }

    if (!positionalArgs.isEmpty())
        frame.selectModule(positionalArgs.at(0));
#ifndef QT_DEBUG
    else if (parser.isSet(showOption))
#endif
    frame.show();

    // setup theme manager
    DThemeManager *manager = DThemeManager::instance();
    QObject::connect(manager, &DThemeManager::themeChanged, [ = ](QString theme) {
        onThemeChange(theme);
    });

    manager->setTheme("dark");
    onThemeChange("dark");

    return app.exec();
}
