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

#include "interface/namespace.h"
#include "modules/sound/soundmodel.h"

#include <DListView>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
DWIDGET_END_NAMESPACE

class TitleLabel;

namespace dcc {

namespace sound {
class SoundModel;
}

namespace widgets {
class SwitchWidget;
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace sound {

class DevicemanagesPage : public QWidget
{
    Q_OBJECT
public:
    explicit DevicemanagesPage(QWidget *parent = nullptr);
    ~DevicemanagesPage();

public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchSetEnable(unsigned int cardId, const QString &cardName, bool enable);

private Q_SLOTS:
    void refreshPort();
    void addPort(const dcc::sound::Port *port);
    void removePort(const QString &portId, const uint &cardId);

protected:
    void initUI();
    void refreshTitleStatus(dcc::sound::Port::Direction direction);

private:
    dcc::sound::SoundModel *m_model{ nullptr };
    QVBoxLayout *m_layout{ nullptr };
    TitleLabel *m_outputDeviceTitle{ nullptr };
    DTipLabel *m_outputlblTip{ nullptr };
    TitleLabel *m_inputDeviceTitle{ nullptr };
    DTipLabel *m_inputlblTip{ nullptr };

    dcc::widgets::SettingsGroup *m_outputGroup{ nullptr };
    dcc::widgets::SettingsGroup *m_inputGroup{ nullptr };

    QList<const dcc::sound::Port *> m_outputPort;
    QList<const dcc::sound::Port *> m_inputPort;
};

}
}
#endif // DEVICEMANAGES_H
