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
#include "updatemodule.h"
#include "modules/update/updatemodel.h"
#include "modules/update/updatework.h"
#include "updatewidget.h"
#include "mirrorswidget.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "modules/systeminfo/systeminfowork.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateModule::UpdateModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_updateWidget(nullptr)
{

}

void UpdateModule::initialize()
{
    m_model = new UpdateModel;
    m_work = new UpdateWorker(m_model);
    m_work->activate(); //refresh data

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

void UpdateModule::active()
{
    UpdateWidget *mainWidget = new UpdateWidget;
    mainWidget->initialize();
    mainWidget->setSystemVersion(getSystemVersion());
    mainWidget->setModel(m_model, m_work);
    m_updateWidget = mainWidget;

    connect(mainWidget, &UpdateWidget::pushMirrorsView, this, [this]() {
        MirrorsWidget *mirrorsWidget = new MirrorsWidget(m_model);

        m_work->checkNetselect();
        mirrorsWidget->setMinimumWidth(350);
        mirrorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWorker::setMirrorSource);
        connect(mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWorker::testMirrorSpeed);

        m_frameProxy->pushWidget(this, mirrorsWidget, FrameProxyInterface::PushType::CoverTop);
    });

    m_frameProxy->pushWidget(this, mainWidget);
    mainWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
}

void UpdateModule::load(QString path)
{
    if (m_updateWidget) {
        if (path == "Update Setting") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSetting);
        } else if (path == "Update") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        } else if (path == "Update Setting/Switch Mirror") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSettingMir);
        }
    }
}

QString UpdateModule::getSystemVersion()
{
    SystemInfoInter *systemInfoInter = new SystemInfoInter("com.deepin.daemon.SystemInfo",
                                                           "/com/deepin/daemon/SystemInfo",
                                                           QDBusConnection::sessionBus(), this);

    return systemInfoInter->distroVer();
}
