/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commoninfowork.h"
#include "window/modules/commoninfo/commoninfomodel.h"

#include "widgets/basiclistdelegate.h"

#include <DSysInfo>

using namespace DCC_NAMESPACE;
using namespace commoninfo;

static const int ItemWidth = 334;
static const int ItemHeight = 177;

CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
{
    m_dBusGrub = new GrubDbus("com.deepin.daemon.Grub2",
                             "/com/deepin/daemon/Grub2",
                             QDBusConnection::systemBus(),
                             this);

    m_dBusGrubTheme = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                       "/com/deepin/daemon/Grub2/Theme",
                                       QDBusConnection::systemBus(), this);

    m_dBusGrub->setSync(false, false);
    m_dBusGrubTheme->setSync(false, false);

    connect(m_dBusGrub, &GrubDbus::DefaultEntryChanged, m_commomModel, &CommonInfoModel::setDefaultEntry);
    connect(m_dBusGrub, &GrubDbus::EnableThemeChanged, m_commomModel, &CommonInfoModel::setThemeEnabled);
    connect(m_dBusGrub, &GrubDbus::TimeoutChanged, this, [this](const int &value) {
        m_commomModel->setBootDelay(value > 1);
    });
    connect(m_dBusGrub, &__Grub2::UpdatingChanged, m_commomModel, &CommonInfoModel::setUpdating);

    connect(m_dBusGrub, &GrubDbus::serviceStartFinished, this, [ = ] {
        QTimer::singleShot(100, this, &CommonInfoWork::grubServerFinished);
    }, Qt::QueuedConnection);

    connect(m_dBusGrubTheme, &GrubThemeDbus::BackgroundChanged, this, &CommonInfoWork::onBackgroundChanged);
}

void CommonInfoWork::activate()
{
}

void CommonInfoWork::deactivate()
{
}

void CommonInfoWork::loadGrubSettings()
{
    if (m_dBusGrub->isValid()) {
        grubServerFinished();
    } else {
        m_dBusGrub->startServiceProcess();
    }
}

void CommonInfoWork::setBootDelay(bool value)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dBusGrub->SetTimeout(value ? 5 : 1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->bootDelayChanged(m_commomModel->bootDelay());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void CommonInfoWork::setEnableTheme(bool value)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dBusGrub->SetEnableTheme(value);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->themeEnabledChanged(m_commomModel->themeEnabled());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void CommonInfoWork::setDefaultEntry(const QString &entry)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dBusGrub->SetDefaultEntry(entry);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->defaultEntryChanged(m_commomModel->defaultEntry());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void CommonInfoWork::grubServerFinished()
{
    m_commomModel->setBootDelay(m_dBusGrub->timeout() > 1);
    m_commomModel->setThemeEnabled(m_dBusGrub->enableTheme());
    m_commomModel->setUpdating(m_dBusGrub->updating());

    getEntryTitles();
    onBackgroundChanged();
}

void CommonInfoWork::onBackgroundChanged()
{
    QDBusPendingCall call = m_dBusGrubTheme->GetBackground();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &CommonInfoWork::getBackgroundFinished);
}

void CommonInfoWork::setBackground(const QString &path)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dBusGrubTheme->SetBackgroundSourceFile(path);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        Q_EMIT requestSetAutoHideDCC(true);

        if (w->isError()) {
            onBackgroundChanged();
        } else {
            setEnableTheme(true);
        }

        w->deleteLater();
    });
}

void CommonInfoWork::setUeProgram(bool enabled)
{
    m_commomModel->setUeProgram(enabled);
}

void CommonInfoWork::getEntryTitles()
{
    QDBusPendingCall call = m_dBusGrub->GetSimpleEntryTitles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (!w->isError()) {
            QDBusReply<QStringList> reply = w->reply();
            QStringList entries = reply.value();
            m_commomModel->setEntryLists(entries);
            m_commomModel->setDefaultEntry(m_dBusGrub->defaultEntry());
        } else {
            qWarning() << "get grub entry list failed : " << w->error().message();
        }

        w->deleteLater();
    });
}

void CommonInfoWork::getBackgroundFinished(QDBusPendingCallWatcher *w)
{
    if (!w->isError()) {
        QDBusPendingReply<QString> reply = w->reply();
        const qreal ratio = qApp->devicePixelRatio();

        QPixmap pix = QPixmap(reply.value()).scaled(QSize(ItemWidth * ratio, ItemHeight * ratio),
                                                    Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        const QRect r(0, 0, ItemWidth * ratio, ItemHeight * ratio);
        const QSize size(ItemWidth * ratio, ItemHeight * ratio);

        if (pix.width() > ItemWidth * ratio || pix.height() > ItemHeight * ratio)
            pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

        pix.setDevicePixelRatio(ratio);

        m_commomModel->setBackground(pix);
    } else {
        qWarning() << w->error().message();
    }

    w->deleteLater();
}
