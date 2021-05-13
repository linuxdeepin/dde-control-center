/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "dnssection.h"
#include "widgets/buttoneditwidget.h"
#include "widgets/settingshead.h"

#include <networkmanagerqt/ipv4setting.h>
#include <networkmanagerqt/ipv6setting.h>

#include <dspinbox.h>
#include <DLineEdit>
#include <DIconButton>
#include <QEvent>

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace NetworkManager;

DNSSection::DNSSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection (parent)
    , m_headItem(new SettingsHead)
    , m_connSettings(connSettings)
    , m_ipv4Setting(m_connSettings->setting(Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>())
    , m_ipv6Setting(m_connSettings->setting(Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>())
{
    initUI();
}

DNSSection::~DNSSection()
{

}

void DNSSection::initUI()
{
    // 添加头 编辑完成 按钮
    m_headItem->setTitle(tr("DNS"));
    m_headItem->toCancel();
    // 需要与列表中文字对齐，列表文字与边框边距为8
    m_headItem ->setContentsMargins(8, 0, 0, 0);
    setSettingsHead(m_headItem);
    connect(m_headItem, &SettingsHead::editChanged, this, &DNSSection::onBtnShow);
    connect(m_headItem, &SettingsHead::editChanged, this, &DNSSection::editClicked);

    // item 为空固定显示两个 edit
    QList<QHostAddress> dnsList = getIPvxDate();
    const int dateNum = dnsList.size();
    if (dateNum <= 2) {
        initDnsList(2);
    } else {
        initDnsList(dateNum);
    }

    onReduceBtnStatusChange(dateNum > 2);
    onBtnShow(false);

    // 获取 DNS 数据信息显示
    for (int i = 0; i < dnsList.size(); i++) {
        // 考虑到 dns 的值可通过配置文件进行修改 防止下标越界， 当i=10说明dnslist的值已经超过10
        if (i >= 10) {
            break;
        } else {
            m_itemsList.at(i)->setText(dnsList.at(i).toString());
        }
    }

    onDnsDateListChanged();
}

void DNSSection::initDnsList(const int &num)
{
    for (int i = 0; i < num; i++) {
        addNewDnsEdit(i);
    }
}

void DNSSection::onDnsDateListChanged()
{
    // 更新btn 状态
    onReduceBtnStatusChange(m_itemsList.size() > 2);
    onAddBtnStatusChange(m_itemsList.size() < 10);

    // 更新 dns 名称
    for (int i = 0; i < m_itemsList.size(); i++) {
        QString dnsName = QString("DNS%1").arg( i+1 );
        m_itemsList.at(i)->setTitle(dnsName);
    }
}

QList<QHostAddress> DNSSection::getIPvxDate()
{
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_ipv4Setting.staticCast<NetworkManager::Ipv4Setting>();
    NetworkManager::Ipv6Setting::Ptr ipv6Setting = m_ipv6Setting.staticCast<NetworkManager::Ipv6Setting>();

    QList<QHostAddress> alldns ;

    if (ipv4Setting) {
        const QList<QHostAddress> &dns4 = ipv4Setting->dns();
        alldns.append(dns4);
    }
    if (ipv6Setting) {
        const QList<QHostAddress> &dns6 = ipv6Setting->dns();
        alldns.append(dns6);
    }

    return alldns;
}


void DNSSection::deleteCurrentDnsEdit(ButtonEditWidget *item)
{
    m_itemsList.removeOne(item);
    this->removeEventFilter(item);
    item->deleteLater();
    onDnsDateListChanged();
}

void DNSSection::addNewDnsEdit(const int &index)
{
    if (m_itemsList.size() == 10) {
        onAddBtnStatusChange(false);
        return;
    }
    ButtonEditWidget *dnsNewSpinbox = new ButtonEditWidget(this);
    insertItem(index,dnsNewSpinbox);
    dnsNewSpinbox->dTextEdit()->lineEdit()->installEventFilter(this);
    dnsNewSpinbox->addBtn()->setFocusPolicy(Qt::NoFocus);
    dnsNewSpinbox->reduceBtn()->setFocusPolicy(Qt::NoFocus);
    m_itemsList.insert(index, dnsNewSpinbox);

    onDnsDateListChanged();

    // 设置 tab 顺序
    for (int i = 0; i < m_itemsList.size() - 1; i++) {
        setTabOrder(m_itemsList.at(i)->dTextEdit(), m_itemsList.at(i+1)->dTextEdit());
    }

    connect(dnsNewSpinbox, &ButtonEditWidget::addNewDnsEdit, this, [=]() {
        addNewDnsEdit(m_itemsList.indexOf(dnsNewSpinbox) + 1);
    });
    connect(dnsNewSpinbox, &ButtonEditWidget::deleteCurrentDnsEdit, this, [=]() {
        deleteCurrentDnsEdit(dnsNewSpinbox);
    });
}

void DNSSection::onAddBtnStatusChange(const bool &flag)
{
    QList<dcc::widgets::ButtonEditWidget *>::iterator iter = m_itemsList.begin();
    for (; iter != m_itemsList.end(); iter++) {
        (*iter)->addBtn()->setEnabled(flag);
    }
}

void DNSSection::onReduceBtnStatusChange(const bool &flag)
{
    QList<dcc::widgets::ButtonEditWidget *>::iterator iter = m_itemsList.begin();
    for (; iter != m_itemsList.end(); iter++) {
        (*iter)->reduceBtn()->setEnabled(flag);
    }
}

void DNSSection::onBtnShow(const bool &edit)
{
    QList<dcc::widgets::ButtonEditWidget *>::iterator iter = m_itemsList.begin();
    for (; iter != m_itemsList.end(); iter++) {
        if (edit) {
            (*iter)->showIconBtn();
        } else {
            (*iter)->hideIconBtn();
        }
    }
}

bool DNSSection::allInputValid()
{
    // 对 DNS 内容进行验证
    bool valid = true;
    QStringList dnsDates;
    for (int i = 0; i < m_itemsList.size(); i++) {
        QString dnsDate = m_itemsList.at(i)->dTextEdit()->text();
        // 合理性验证
        if (isIpv4Address(dnsDate) || isIpv6Address(dnsDate)) {
            dnsDates.append(dnsDate);
            m_itemsList.at(i)->setIsErr(false);
         } else {
            if (!dnsDate.isEmpty()) {
                valid = false;
                dnsDates.clear();
                m_itemsList.at(i)->setIsErr(true);
                m_itemsList.at(i)->dTextEdit()->setAlert(true);
                m_itemsList.at(i)->dTextEdit()->showAlertMessage(tr("Invalid DNS address"), parentWidget(), 2000);
            }
        }
    }

    // 将合格数据分类保存
    if (valid) {
        for (int i = 0; i < dnsDates.size(); i++) {
            QString date = dnsDates.at(i);
            if (isIpv4Address(date))
                m_ipv4Dns.append(QHostAddress(date));
            if (isIpv6Address(date))
                m_ipv6Dns.append(QHostAddress(date));
        }
    }

    return valid;
}

void DNSSection::saveSettings()
{
    // TODO： ipvX自动认证未输入直接忽略
    if (m_ipv4Setting) {
        NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_ipv4Setting.staticCast<NetworkManager::Ipv4Setting>();
        ipv4Setting->setIgnoreAutoDns(!m_ipv4Dns.isEmpty());
        ipv4Setting->setDns(m_ipv4Dns);
        m_ipv4Setting->setInitialized(true);
    }

    if (m_ipv6Setting) {
        NetworkManager::Ipv6Setting::Ptr ipv6Setting = m_ipv6Setting.staticCast<NetworkManager::Ipv6Setting>();
        ipv6Setting->setIgnoreAutoDns(!m_ipv6Dns.isEmpty());
        ipv6Setting->setDns(m_ipv6Dns);
        m_ipv6Setting->setInitialized(true);
    }
}

bool DNSSection::isIpv4Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) {
        return false;
    }
    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    return regExpIP.exactMatch(ip);
}

bool DNSSection::isIpv6Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv6)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv6Protocol) {
        return false;
    }
    if (ipAddr == QHostAddress(QHostAddress::LocalHostIPv6)) {
        return false;
    }
    return true;
}

bool DNSSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched) || dynamic_cast<QSpinBox *>(watched)) {
            Q_EMIT editClicked();
        }
    }
    return QWidget::eventFilter(watched, event);
}
