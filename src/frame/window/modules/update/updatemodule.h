/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "window/interface/moduleinterface.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateWorker;
class UpdateModel;
class MirrorsWidget;
}
}

namespace DCC_NAMESPACE {
namespace update {
class UpdateWidget;

class UpdateModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    UpdateModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual void active() override;
    virtual void load(QString path) override;
    QString getSystemVersion();

private:

private:
    dcc::update::UpdateModel *m_model;
    dcc::update::UpdateWorker *m_work;
    QPointer<UpdateWidget> m_updateWidget;

Q_SIGNALS:

public Q_SLOTS:
};

}// namespace datetime
}// namespace DCC_NAMESPACE
