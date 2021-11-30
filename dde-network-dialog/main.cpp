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

#include "localclient.h"
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

using namespace Dtk::Widget;

const int MAX_STACK_FRAMES = 128;
void sig_crash(int sig)
{
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
    QCommandLineOption showOption(QStringList() << "s", "show config", "config");
    QCommandLineOption wepOption(QStringList() << "w", "wireless wep-key");
    QCommandLineOption connectPathOption(QStringList() << "c", "connect wireless ", "path");
    QCommandLineOption devOption(QStringList() << "n", "network device", "device");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Network Dialog");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(wepOption);
    parser.addOption(connectPathOption);
    parser.addOption(devOption);
    parser.process(*qApp);

    if (parser.isSet(showOption)) {
        QString config = parser.value(showOption);
        LocalClient::instance()->showPosition(nullptr, config.toUtf8());
    }
    if (parser.isSet(wepOption)) {
        QString dev = parser.value(devOption);
        QString ssid = parser.value(connectPathOption);
        LocalClient::instance()->waitPassword(dev, ssid);
    } else {
        LocalClient::instance()->showWidget();
    }
    return app->exec();
}
