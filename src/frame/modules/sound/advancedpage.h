/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef DCC_SOUND_ADVANCEDPAGE_H
#define DCC_SOUND_ADVANCEDPAGE_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "portitem.h"
#include "soundmodel.h"

namespace dcc {
namespace sound {

class AdvancedPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit AdvancedPage(SoundModel *model);
    ~AdvancedPage();

    void setModel(SoundModel *model);

Q_SIGNALS:
    void requestSetPort(const Port *port);

private Q_SLOTS:
    void addPort(const Port *port);
    void removePort(const QString &portId, const uint &cardId);

private:
    SoundModel *m_model;
    SettingsGroup *m_outputGroup;
    SettingsGroup *m_inputGroup;
    QList<PortItem *> m_portItemList;
};

} // namespace sound
} // namespace dcc

#endif // DCC_SOUND_ADVANCEDPAGE_H
