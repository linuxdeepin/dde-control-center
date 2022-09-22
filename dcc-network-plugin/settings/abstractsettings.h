// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ABSTRACTSETTINGS_H
#define ABSTRACTSETTINGS_H

#include <QWidget>

#include <KF5/NetworkManagerQt/networkmanagerqt/connectionsettings.h>

namespace dcc {
  class ContentWidget;
}

class AbstractSection;
class QVBoxLayout;

using namespace dcc;
using namespace NetworkManager;

class AbstractSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~AbstractSettings();

    bool allInputValid();
    void saveSettings();

    bool isAutoConnect();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const );
    void requestFrameAutoHide(const bool autoHide) const;
    void anyEditClicked();

protected:
    virtual void initSections() = 0;

    // 是否清除连接的 interface-name 字段
    // 如果不设置连接的 macAddress 则需要清空 interface-name 字段, 反之不清空, 而是设置为当前设备的 interfaceName
    virtual bool clearInterfaceName() = 0;

protected:
    virtual void resetConnectionInterfaceName();

protected:
    ConnectionSettings::Ptr m_connSettings;

    QVBoxLayout *m_sectionsLayout;
    QList<AbstractSection *> m_settingSections;
};

#endif /* ABSTRACTSETTINGS_H */
