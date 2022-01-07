/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#include "dslcontroller.h"
#include "hotspotcontroller.h"
#include "networkcontroller.h"
#include "networkdetails.h"
#include "networkdevicebase.h"
#include "proxycontroller.h"
#include "vpncontroller.h"
#include "wireddevice.h"
#include "wirelessdevice.h"
#include "netutils.h"
#include "realize/networkmanagerprocesser.h"
#include "realize/networkinterprocesser.h"

const static QString networkService = "com.deepin.daemon.Network";
const static QString networkPath = "/com/deepin/daemon/Network";

using namespace dde::network;

// 默认是异步方式
bool NetworkController::m_sync = false;
// 只有任务栏需要检测IP冲突，因此任务栏需要调用相关的接口来检测，其他的应用是不需要检测冲突的
bool NetworkController::m_checkIpConflicted = false;
// 默认从网络后台检测
ServiceLoadType NetworkController::m_serviceLoadType = ServiceLoadType::LoadFromInter;

NetworkController::NetworkController()
    : QObject(Q_NULLPTR)
    , m_proxyController(Q_NULLPTR)
    , m_vpnController(Q_NULLPTR)
    , m_dslController(Q_NULLPTR)
    , m_hotspotController(Q_NULLPTR)
{
    if (m_serviceLoadType == ServiceLoadType::LoadFromManager)
        m_processer = new NetworkManagerProcesser(this);
    else
        m_processer = new NetworkInterProcesser(m_sync, m_checkIpConflicted, this);

    connect(m_processer, &NetworkProcesser::deviceAdded, this, &NetworkController::deviceAdded);
    connect(m_processer, &NetworkProcesser::deviceRemoved, this, &NetworkController::deviceRemoved);
    connect(m_processer, &NetworkProcesser::connectivityChanged, this, &NetworkController::connectivityChanged);
    connect(m_processer, &NetworkProcesser::connectionChanged, this, &NetworkController::connectionChanged);
    connect(m_processer, &NetworkProcesser::activeConnectionChange, this, &NetworkController::activeConnectionChange);
}

NetworkController::~NetworkController()
{
}

NetworkController *NetworkController::instance()
{
    static NetworkController instance;
    return &instance;
}

void NetworkController::setActiveSync(const bool sync)
{
    m_sync = sync;
}

void NetworkController::setServiceType(const ServiceLoadType serviceType)
{
    m_serviceLoadType = serviceType;
}

void NetworkController::setIPConflictCheck(const bool &checkIp)
{
    m_checkIpConflicted = checkIp;
}

void NetworkController::updateSync(const bool sync)
{
    NetworkInterProcesser *processer = qobject_cast<NetworkInterProcesser *>(m_processer);
    if (processer)
        processer->updateSync(sync);
}

ProxyController *NetworkController::proxyController()
{
    return m_processer->proxyController();
}

VPNController *NetworkController::vpnController()
{
    return m_processer->vpnController();
}

DSLController *NetworkController::dslController()
{
    return m_processer->dslController();
}

HotspotController *NetworkController::hotspotController()
{
    return m_processer->hotspotController();
}

QList<NetworkDetails *> NetworkController::networkDetails()
{
    return m_processer->networkDetails();
}

QList<NetworkDeviceBase *> NetworkController::devices() const
{
    return m_processer->devices();
}

Connectivity NetworkController::connectivity()
{
    return m_processer->connectivity();
}

