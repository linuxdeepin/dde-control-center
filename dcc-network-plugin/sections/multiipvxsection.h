/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@gmail.com>
 *
 * Maintainer: donghualin <donghualin@gmail.com>
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

#ifndef MULTIIPVXSECTION_H
#define MULTIIPVXSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/ipv4setting.h>
#include <networkmanagerqt/ipv6setting.h>
#include <networkmanagerqt/generictypes.h>
#include <networkmanagerqt/ipaddress.h>

namespace dcc {
  namespace widgets {
    class ComboxWidget;
    class LineEditWidget;
    class SpinBoxWidget;
    class SwitchWidget;
    class SettingsHead;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class QComboBox;
class ActionButton;
class IPInputSection;
class QPaintEvent;

class MultiIpvxSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit MultiIpvxSection(Setting::Ptr ipvSetting, QFrame *parent = nullptr);
    virtual ~MultiIpvxSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void refreshItems();
    void addIPV4Config();
    void addIPV6Config();
    void setDefaultValue();
    QList<IPInputSection *> createIpInputSections();
    void setIpInputSection(IPInputSection *ipSection, IPInputSection *itemBefore = nullptr);

protected slots:
    void onDeleteItem(IPInputSection *item);
    void onAddItem(IPInputSection *item);
    void onIPV4OptionChanged();
    void onIPV6OptionChanged();
    void onButtonShow(bool edit);

private:
    QList<IPInputSection *> m_ipSections;

    Setting::Ptr m_ipvxSetting;
    QFrame *m_mainFrame;
    QComboBox *m_methodChooser;
    ComboxWidget *m_methodLine;
    QMap<QString, Ipv4Setting::ConfigMethod> Ipv4ConfigMethodStrMap;
    QMap<QString, Ipv6Setting::ConfigMethod> Ipv6ConfigMethodStrMap;
    SettingsHead *m_headerEditWidget;
    SettingsHead *m_headerWidget;
    bool m_isEditMode;
};

class IPInputSection : public SettingsItem
{
    Q_OBJECT

public:
    explicit IPInputSection(IpAddress ipAddress, QFrame *parent = Q_NULLPTR);
    ~IPInputSection() override;

    void setTtile(const QString &title);
    void setAddItemVisible(bool visible);
    void setDeleteItemVisible(bool visible);
    virtual bool allInputValid(const QList<IpAddress> &ipAddresses) = 0;
    virtual IpAddress ipAddress() { return IpAddress(); }

Q_SIGNALS:
    void editClicked();
    void requestDelete(IPInputSection *);
    void requestAdd(IPInputSection *);

protected:
    void initUi();
    void initConnection();
    bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    LineEditWidget *m_lineIpAddress;
    LineEditWidget *m_gateway;
    QVBoxLayout *m_mainLayout;
    IpAddress m_ipAddress;

private:
    QWidget *m_headerWidget;
    DLabel *m_titleLabel;

    ActionButton *m_newIpButton;
    QLabel *m_pSpaceLabel;
    ActionButton *m_deleteButton;
};

class IPV4InputSection : public IPInputSection
{
    Q_OBJECT

public:
    explicit IPV4InputSection(IpAddress ipAddress, QFrame *parent = Q_NULLPTR);
    ~IPV4InputSection() override;
    bool allInputValid(const QList<IpAddress> &ipAddresses) override;

private:
    void initUi();
    void initConnection();
    bool isIpv4Address(const QString &ip);
    bool isIpv4SubnetMask(const QString &ip);
    IpAddress ipAddress() override;

private:
    QMap<QString, Ipv4Setting::ConfigMethod> Ipv4ConfigMethodStrMap;
    LineEditWidget *m_netmaskIpv4;
};

class IPV6InputSection : public IPInputSection
{
    Q_OBJECT

public:
    explicit IPV6InputSection(IpAddress ipAddress, QFrame *parent = Q_NULLPTR);
    ~IPV6InputSection() override;
    bool allInputValid(const QList<IpAddress> &ipAddresses) override;
    IpAddress ipAddress() override;

protected:
    void initUi();
    bool isIpv6Address(const QString &ip);

private:
    SpinBoxWidget *m_prefixIpv6;
};

#endif /* MULTIIPVXSECTION_H */
