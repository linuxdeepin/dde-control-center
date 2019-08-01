/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef ADVANCEDPAGE_H_V20
#define ADVANCEDPAGE_H_V20

#include "../../namespace.h"

#include <QWidget>

class QVBoxLayout;

namespace dcc {

namespace sound {
class SoundModel;
class PortItem;
class Port;
}

namespace widgets {
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class AdvancedPage : public QWidget
{
    Q_OBJECT
public:
    AdvancedPage(QWidget *parent = nullptr);
    ~AdvancedPage();
public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSetPort(const dcc::sound::Port *);

private Q_SLOTS:
    void removePort(const QString &portId, const uint &cardId);
    void addPort(const dcc::sound::Port *port);

private:
    void initList();

private:
    QVBoxLayout *m_layout;
    dcc::sound::SoundModel *m_model{nullptr};
    dcc::widgets::SettingsGroup *m_inputGroup{nullptr};
    dcc::widgets::SettingsGroup *m_outputGroup{nullptr};
    QList<dcc::sound::PortItem *> m_portItemList;
};

}
}
#endif // ADVANCEDPAGE_H_V20
