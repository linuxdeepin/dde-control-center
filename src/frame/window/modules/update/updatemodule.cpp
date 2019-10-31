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
    , m_mirrorsWidget(nullptr)
{

}

void UpdateModule::initialize()
{

}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

void UpdateModule::active()
{
    if (!m_model)
        m_model = new UpdateModel(this);

    if (!m_work) {
        m_work = new UpdateWorker(m_model);
        m_work->moveToThread(qApp->thread());
        m_model->moveToThread(qApp->thread());
    }

    m_work->activate(); //refresh data

    UpdateWidget *mainWidget = new UpdateWidget;
    mainWidget->initialize();
    mainWidget->setSystemVersion(m_model->systemVersionInfo());
    mainWidget->setModel(m_model, m_work);
    m_updateWidget = mainWidget;

    connect(m_model, &UpdateModel::downloadInfoChanged, m_work, &UpdateWorker::onNotifyDownloadInfoChanged);

    connect(mainWidget, &UpdateWidget::pushMirrorsView, this, [this]() {
        m_mirrorsWidget = new MirrorsWidget(m_model);

        m_work->checkNetselect();
        m_mirrorsWidget->setMinimumWidth(350);
        m_mirrorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWorker::setMirrorSource);
        connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWorker::testMirrorSpeed);
        connect(m_mirrorsWidget, &MirrorsWidget::notifyDestroy, this, [this]() {
            //notifyDestroy信号是此对象被销毁，析构时发出的，资源销毁了要将其对象赋值为空
            m_mirrorsWidget = nullptr;
        });
        connect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);

        m_frameProxy->pushWidget(this, m_mirrorsWidget);
    });

    m_frameProxy->pushWidget(this, mainWidget);
    mainWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
}

void UpdateModule::deactive()
{
    if (m_model) {
        m_model->deleteLater();
        m_model = nullptr;
    }

    if (m_work) {
        m_work->deleteLater();
        m_work = nullptr;
    }
}

void UpdateModule::load(QString path)
{
    if (m_updateWidget) {
        if (path == "Update Settings") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSetting);
        } else if (path == "Update") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        } else if (path == "Update Settings/Mirror List") {
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSettingMir);
        }
    }
}

void UpdateModule::onNotifyDealMirrorWidget(bool state)
{
    //m_mirrorsWidget存在表示有第三级页面
    if (state && m_mirrorsWidget) {
        m_frameProxy->popWidget(this);
        //popWidget之后就没有第三级页面了,即m_mirrorsWidget为空指针,需要对其地址赋值为nullptr
        //假如是从开启镜像源列表页面后，切换其他页面此时这里资源已经被释放了，但是指针没有赋值为空；再次进入，关闭此处就会直接返回主页面(接收析构时信号)
        m_mirrorsWidget = nullptr;
        //避免第三级页面不存在后,还会处理该函数
        disconnect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);
    }
}
