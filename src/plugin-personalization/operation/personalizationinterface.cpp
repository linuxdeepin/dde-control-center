//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "personalizationinterface.h"
#include "operation/imagehelper.h"
#include "operation/personalizationexport.hpp"
#include "operation/treelandworker.h"
#include "operation/x11worker.h"
#include "dccfactory.h"
#include "model/thememodel.h"
#include "model/wallpapermodel.h"
#include "utils.hpp"

#include <QIcon>
#include <QtQml>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QUuid>
#include <QColor>
#include <DGuiApplicationHelper>

#define PICKER_SERVICE "com.deepin.Picker"
#define PICKER_PATH "/com/deepin/Picker"

PersonalizationInterface::PersonalizationInterface(QObject *parent) 
: QObject(parent)
, m_model(new PersonalizationModel(this))
, m_imageHelper(new ImageHelper(this))
, m_pickerId(QUuid::createUuid().toString())
, m_pickerAvailable(false)
{
    qmlRegisterType<PersonalizationExport>("org.deepin.dcc.personalization", 1, 0, "PersonalizationData");
    if (Dtk::Gui::DGuiApplicationHelper::testAttribute(Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform)) {
        m_work = new TreeLandWorker(m_model, this);
    } else {
        m_work = new X11Worker(m_model, this);
    }

    qmlRegisterUncreatableMetaObject(WallpaperEnums::staticMetaObject, "org.deepin.dcc.personalization", 1, 0, "WallpaperEnums", "WallpaperEnums namespace");
    qmlRegisterType<PersonalizationExport>("org.deepin.dcc.personalization", 1, 0, "PersonalizationExport");

    // after do
    m_work->active(); // refresh data
    m_work->refreshTheme();
    m_work->refreshFont();

    m_pickerAvailable = checkPickerService();
    if (m_pickerAvailable) {
        QDBusConnection::sessionBus().connect(PICKER_SERVICE, PICKER_PATH, PICKER_SERVICE,
                                              "colorPicked", this, SLOT(onPickerColorPicked(QString,QString)));
        qDebug() << "Picker service is available, using DBus picker";
    } else {
        qDebug() << "Picker service is not available, will use Qt's built-in color picker";
    }
}

QString PersonalizationInterface::platformName()
{
    return qApp->platformName();
}

void PersonalizationInterface::handleCmdParam(PersonalizationExport::ModuleType type, const QString &cmdParam)
{
    // parse cmd param
    QMap<QString, QString> paramMap;
    QStringList paramPairs = cmdParam.split('&');

    for (const auto &paramPair : paramPairs) {
        QStringList keyValue = paramPair.split('=');

        if (keyValue.size() == 2) {
            QString key = keyValue[0].trimmed();
            QString value = keyValue[1].trimmed();
            paramMap.insert(key, value);
        }
    }

    // handle cmd param
    if (type == PersonalizationExport::Theme) {
        QString operatorType;
        QString value;
        operatorType = paramMap.value("type");
        value = paramMap.value("value");

        if (operatorType == "themeType") {
            bool keepAuto = paramMap.value("keepAuto") == "true";
            if (value == "light") {
                m_work->setAppearanceTheme(".light", keepAuto);
            } else if (value == "dark") {
                m_work->setAppearanceTheme(".dark", keepAuto);
            } else if (value == "auto") {
                m_work->setAppearanceTheme("", keepAuto);
            }
        }
    } else if (type == PersonalizationExport::Wallpaper) {
        QString url;
        QString type;
        QString monitor;
        url = paramMap.value("url");
        if (!isURI(url) && QFile::exists(url)) {
            url = QUrl::fromLocalFile(url).toString();
        }
        type = paramMap.value("type");
        monitor = paramMap.value("monitor");
        if (monitor.isEmpty()) {
            monitor = m_model->getCurrentSelectScreen();
        }
        if (url.isEmpty()) {
            return;
        }
        if (type == "lock") {
            m_work->setLockBackForMonitor(monitor, url, true);
        } else if (type == "desktop") {
            m_work->setBackgroundForMonitor(monitor, url, true);
        } else if (type.isEmpty()) {
            m_work->setLockBackForMonitor(monitor, url, true);
            m_work->setBackgroundForMonitor(monitor, url, true);
        }
    }
}

bool PersonalizationInterface::checkPickerService()
{
    QDBusConnectionInterface *interface = QDBusConnection::sessionBus().interface();
    if (!interface) {
        return false;
    }

    if (interface->isServiceRegistered(PICKER_SERVICE)) {
        return true;
    }

    QStringList activatableServices = interface->activatableServiceNames();
    return activatableServices.contains(PICKER_SERVICE);
}

void PersonalizationInterface::startPicker()
{
    if (!m_pickerAvailable) {
        qWarning() << "Picker service is not available";
        Q_EMIT pickerError(tr("Picker service is not available"));
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(PICKER_SERVICE, PICKER_PATH,
                                                      PICKER_SERVICE, "StartPick");
    msg.setArguments({QVariant::fromValue(m_pickerId)});
    QDBusConnection::sessionBus().asyncCall(msg, 5);

    qDebug() << "Picker service called with ID:" << m_pickerId;
}

void PersonalizationInterface::onPickerColorPicked(const QString &uuid, const QString &colorName)
{
    if (uuid != m_pickerId)
        return;

    qDebug() << "Picked color:" << colorName << "for session:" << uuid;

    QColor color(colorName);
    if (color.isValid()) {
        Q_EMIT colorPicked(color.name());
    } else {
        Q_EMIT pickerError(tr("Invalid color format: %1").arg(colorName));
    }
}

DCC_FACTORY_CLASS(PersonalizationInterface)

#include "personalizationinterface.moc"
