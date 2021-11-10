/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "networkpanel.h"
#include "dockpopupwindow.h"
#include "localclient.h"
#include "localserver.h"
#include "utils.h"
#include "thememanager.h"
#include "networkcontroller.h"

#include <DApplication>
#include <DConfig>
#include <DPlatformTheme>
#include <DLog>

#include <QWindow>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTranslator>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>

#include <execinfo.h>
#include <sys/stat.h>
#include <signal.h>

const int MAX_STACK_FRAMES = 128;
void sig_crash(int sig)
{
    LocalServer::release(); // 异常退出时释放
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0]);
    dir.cdUp();
    QString filePath = "/tmp/dde-collapse.log";

    QFile *file = new QFile(filePath);

    if (!file->open(QIODevice::Text | QIODevice::Append)) {
        qDebug() << file->errorString();
        exit(0);
    }

    if (file->size() >= 10 * 1024 * 1024) {
        // 清空原有内容
        file->close();
        if (file->open(QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << file->errorString();
            exit(0);
        }
    }

    // 捕获异常，打印崩溃日志到配置文件中
    try {
        QString head = "\n#####" + qApp->applicationName() + "#####\n"
                       + QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]")
                       + "[crash signal number:" + QString::number(sig) + "]\n";
        file->write(head.toUtf8());

#ifdef Q_OS_LINUX
        void *array[MAX_STACK_FRAMES];
        size_t size = 0;
        char **strings = nullptr;
        size_t i;
        signal(sig, SIG_DFL);
        size = static_cast<size_t>(backtrace(array, MAX_STACK_FRAMES));
        strings = backtrace_symbols(array, int(size));
        for (i = 0; i < size; ++i) {
            QString line = QString::number(i) + " " + QString::fromStdString(strings[i]) + "\n";
            file->write(line.toUtf8());

            std::string symbol(strings[i]);
            QString strSymbol = QString::fromStdString(symbol);
            int pos1 = strSymbol.indexOf("[");
            int pos2 = strSymbol.lastIndexOf("]");
            QString address = strSymbol.mid(pos1 + 1, pos2 - pos1 - 1);

            // 按照内存地址找到对应代码的行号
            QString cmd = "addr2line -C -f -e " + qApp->applicationName() + " " + address;
            QProcess *p = new QProcess;
            p->setReadChannel(QProcess::StandardOutput);
            p->start(cmd);
            p->waitForFinished();
            p->waitForReadyRead();
            file->write(p->readAllStandardOutput());
            delete p;
            p = nullptr;
        }
        free(strings);
#endif // __linux
    } catch (...) {
        //
    }
    file->close();
    delete file;
    file = nullptr;
    exit(0);
}

void init_sig_crash()
{
    signal(SIGTERM, sig_crash);
    signal(SIGSEGV, sig_crash);
    signal(SIGILL, sig_crash);
    signal(SIGINT, sig_crash);
    signal(SIGABRT, sig_crash);
    signal(SIGFPE, sig_crash);
}

class MainApp
{
public:
    enum RunReason {
        Lock,      // 锁屏插件唤起
        Greeter,   // greeter插件唤起
        NeedFocus, // 小于该值需要处理焦点
        Dock,      // 任务栏插件唤起
        Password,  // 密码错误唤起
    };

    MainApp()
        : m_isWep(false)
        , m_isSave(false)
        , m_reason(Dock)
    {
    }
    ~MainApp() { }

    void run();
    // 解析参数
    void parseArguments();
    void saveConfig();
    bool clientModel();
    void showWidget();

private:
    DArrowRectangle::ArrowDirection m_position;
    QPoint m_point;
    QString m_path;
    bool m_isWep;
    bool m_isSave;
    RunReason m_reason;
};

void MainApp::run()
{
    parseArguments();
    if (m_isSave) {
        saveConfig();
        qApp->exit(0);
    } else if (!clientModel()) {
        saveConfig();
        showWidget();
    }
}

void MainApp::parseArguments()
{
    QCommandLineOption pointOption(QStringList() << "p", "set point <x>x<y>", "point");
    QCommandLineOption positionOption(QStringList() << "d", "position [l,r,t,b]", "position");
    QCommandLineOption connectPathOption(QStringList() << "c"
                                                       << "connect",
                                         "connect wireless ", "path");
    QCommandLineOption reasonOption(QStringList() << "r"
                                                  << "reason",
                                    "run reason", "reason");
    QCommandLineOption wepOption(QStringList() << "w", "wireless wep-key");
    QCommandLineOption saveOption(QStringList() << "s", "save config", "save");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Network Dialog");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(pointOption);
    parser.addOption(positionOption);
    parser.addOption(connectPathOption);
    parser.addOption(reasonOption);
    parser.addOption(wepOption);
    parser.addOption(saveOption);
    parser.process(*qApp);

    Dtk::Core::DConfig config("dde-network-dialog");
    QString point;
    if (parser.isSet(pointOption)) {
        point = parser.value(pointOption);
    } else {
        point = config.value("networkDialogPoint", "200x600").toString();
    }
    const QStringList &points = point.split("x");
    if (2 == points.size()) {
        bool xok = false;
        bool yok = false;
        int px = points.first().toInt(&xok);
        int py = points.last().toInt(&yok);
        if (xok && yok) {
            m_point.setX(px);
            m_point.setY(py);
        }
    }

    QString positionStr = "b";
    if (parser.isSet(positionOption)) {
        positionStr = parser.value(positionOption);
    } else {
        positionStr = config.value("networkDialogPosition", "bottom").toString();
    }
    if (!positionStr.isEmpty()) {
        switch (positionStr.at(0).toLower().toLatin1()) {
        case 'b':
            m_position = DArrowRectangle::ArrowBottom;
            break;
        case 't':
            m_position = DArrowRectangle::ArrowTop;
            break;
        case 'l':
            m_position = DArrowRectangle::ArrowLeft;
            break;
        case 'r':
            m_position = DArrowRectangle::ArrowRight;
            break;
        }
    }

    if (parser.isSet(connectPathOption)) {
        QString path = parser.value(connectPathOption);
        m_path = path;
    }

    m_isWep = parser.isSet(wepOption);
    m_isSave = parser.isSet(saveOption);
    if (parser.isSet(reasonOption)) {
        QString reason = parser.value(reasonOption);
        if (reason == "Lock") {
            m_reason = Lock;
        } else if (reason == "Greeter") {
            m_reason = Greeter;
        } else if (reason == "Dock") {
            m_reason = Dock;
        } else if (reason == "Password") {
            m_reason = Password;
        }
    }
}

void MainApp::saveConfig()
{
    Dtk::Core::DConfig config("dde-network-dialog");
    if (!m_point.isNull()) {
        config.setValue("networkDialogPoint", QString("%1x%2").arg(m_point.x()).arg(m_point.y()));
    }
    switch (m_position) {
    case DArrowRectangle::ArrowBottom:
        config.setValue("networkDialogPosition", "bottom");
        break;
    case DArrowRectangle::ArrowTop:
        config.setValue("networkDialogPosition", "top");
        break;
    case DArrowRectangle::ArrowLeft:
        config.setValue("networkDialogPosition", "left");
        break;
    case DArrowRectangle::ArrowRight:
        config.setValue("networkDialogPosition", "right");
        break;
    }
}

bool MainApp::clientModel()
{
    LocalClient *client = new LocalClient(qAppName(), qApp);
    if (client->ConnectToServer()) {
        if (m_isWep) {
            client->waitPassword(m_path);
        }
        return true;
    }
    delete client;
    return false;
}

void MainApp::showWidget()
{
    switch (m_reason) {
    case Greeter:
        dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
        // 此处没break, Greeter的样式处理同Lock
    case Lock:
        ThemeManager::instance()->setThemeType(ThemeManager::LoginType);
        break;
    default:
        break;
    }

    LocalServer::instance()->RunServer(qAppName());
    DockPopupWindow *popopWindow = new DockPopupWindow();
    NetworkPanel *panel = new NetworkPanel(popopWindow);
    popopWindow->setContent(panel->itemApplet());
    QObject::connect(qApp, &DApplication::destroyed, popopWindow, &DockPopupWindow::deleteLater);
    QObject::connect(popopWindow, &DockPopupWindow::hideSignal, qApp, &QApplication::quit);
    QObject::connect(popopWindow, &DockPopupWindow::hideSignal, popopWindow, &DockPopupWindow::deleteLater);
    popopWindow->setArrowDirection(m_position);
    LocalServer::instance()->setWidget(panel, popopWindow);
    if (!m_path.isEmpty()) {
        LocalServer::instance()->setWaitPassword(m_isWep);
        panel->passwordError(m_path);
    }
    popopWindow->show(m_point);
}

int main(int argc, char **argv)
{
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }
    DApplication *app = DApplication::globalApplication(argc, argv);
    app->setOrganizationName("deepin");
    app->setApplicationName("dde-network-dialog");
    app->setQuitOnLastWindowClosed(true);

    QTranslator translator;
    translator.load("/usr/share/dde-network-dialog/translations/dde-network-dialog_" + QLocale::system().name());
    qApp->installTranslator(&translator);

    qApp->setApplicationDisplayName("NetworkDialog");
    qApp->setApplicationDescription("network dialog");

    // crash catch
    init_sig_crash();

    MainApp mainapp;
    mainapp.run();
    int ret = app->exec();
    LocalServer::release();
    return ret;
}
