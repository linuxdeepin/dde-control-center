/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef ABSTRACTSETTINGS_H
#define ABSTRACTSETTINGS_H

#include "widgets/contentwidget.h"
#include "../sections/abstractsection.h"

#include <KF5/NetworkManagerQt/networkmanagerqt/connectionsettings.h>

#include <QWidget>

namespace dcc {
namespace network {

class AbstractSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~AbstractSettings();

    bool allInputValid();
    void saveSettings();

    bool isAutoConnect();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page);
    void requestFrameAutoHide(const bool autoHide) const;

protected:
    virtual void initSections() = 0;

    // 是否清除连接的 interface-name 字段
    // 如果不设置连接的 macAddress 则需要清空 interface-name 字段, 反之不清空, 而是设置为当前设备的 interfaceName
    virtual bool clearInterfaceName() = 0;

private:
    void resetConnectionInterfaceName();

protected:
    NetworkManager::ConnectionSettings::Ptr m_connSettings;

    QVBoxLayout *m_sectionsLayout;
    QList<dcc::network::AbstractSection *> m_settingSections;
};

} /* network */
} /* dcc */

#endif /* ABSTRACTSETTINGS_H */
