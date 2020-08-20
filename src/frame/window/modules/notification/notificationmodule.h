/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#pragma once

#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace notification {
class NotificationWorker;
class NotificationModel;
}
}

namespace DCC_NAMESPACE {
namespace notification {

class NotificationWidget;

class NotificationModule: public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    NotificationModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);
    ~NotificationModule() override;

    virtual void preInitialize() override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

private Q_SLOTS:
    void showSystemNotify();
    void showAppNotify(int index);
private:
    dcc::notification::NotificationModel *m_model;
    dcc::notification::NotificationWorker *m_worker;
    QPointer<NotificationWidget> m_widget;
};

}// namespace msgnotify
}// namespace DCC_NAMESPACE
