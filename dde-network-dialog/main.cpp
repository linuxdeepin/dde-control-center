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

#include <DApplication>
#include <DConfig>
#include <DPlatformTheme>

#include <QWindow>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTranslator>
#include <QTimer>

class MainApp
{
public:
    MainApp() {}
    ~MainApp() {}

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
    bool m_isLogin;
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
    QCommandLineOption loginOption(QStringList() << "l", "login style");
    QCommandLineOption wepOption(QStringList() << "w", "wireless wep-key");
    QCommandLineOption saveOption(QStringList() << "s", "save config", "save");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Network Dialog");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(pointOption);
    parser.addOption(positionOption);
    parser.addOption(connectPathOption);
    parser.addOption(loginOption);
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
    m_isLogin = parser.isSet(loginOption);
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
    translator.load("/usr/share/dde-network-connect/translations/dde-network-dialog_" + QLocale::system().name());
    qApp->installTranslator(&translator);

    qApp->setApplicationDisplayName(QObject::tr("NetworkDialog"));
    qApp->setApplicationDescription(QApplication::translate("main", "network dialog."));

    MainApp mainapp;
    mainapp.run();
    return app->exec();
}
