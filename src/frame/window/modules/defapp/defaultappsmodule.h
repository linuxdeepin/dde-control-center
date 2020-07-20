/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "modules/defapp/defappworker.h"

namespace dcc {
namespace defapp {
class DefAppModel;
class DefAppWorker;
}
}
namespace DCC_NAMESPACE {
namespace defapp {
class DefaultAppsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DefaultAppsModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~DefaultAppsModule();

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual void contentPopped(QWidget *const w) override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

Q_SIGNALS:
    void requestSetDefappCategory(dcc::defapp::DefAppWorker::DefaultAppsCategory category);

private Q_SLOTS:
    void showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category = dcc::defapp::DefAppWorker::Browser);

private:
    dcc::defapp::DefAppModel *m_defAppModel{nullptr};
    dcc::defapp::DefAppWorker *m_defAppWorker{nullptr};
};
}
}
