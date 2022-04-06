/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
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

#ifndef DEVICEMANAGES_H
#define DEVICEMANAGES_H

#include <dtkwidget_global.h>
#include "interface/namespace.h"
#include "soundmodel.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DTipLabel;
DWIDGET_END_NAMESPACE


DCC_BEGIN_NAMESPACE
class TitledSliderItem;
class SettingsGroup;
DCC_END_NAMESPACE

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

class SoundModel;
class DevicemanagesPage : public QWidget
{
    Q_OBJECT
public:
    explicit DevicemanagesPage(Port::Direction direction, QWidget *parent = nullptr);
    ~DevicemanagesPage();

public:
    void setModel(SoundModel *model);

Q_SIGNALS:
    void requestSwitchSetEnable(unsigned int cardId, const QString &cardName, bool enable);

private Q_SLOTS:
    void refreshPort();
    void addPort(const Port *port);
    void removePort(const QString &portId, const uint &cardId);

protected:
    void initUI();

private:
    Port::Direction m_direction;
    SoundModel *m_model{ nullptr };
    QVBoxLayout *m_layout{ nullptr };

    SettingsGroup *m_deviceGroup{ nullptr };
    QList<const Port *> m_devicePort;
};

#endif // DEVICEMANAGES_H
