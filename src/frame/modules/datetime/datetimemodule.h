/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DATETIMEMODULE_H
#define DATETIMEMODULE_H

#include "interfaces/moduleinterface.h"
#include "datetime.h"
#include "datetimework.h"
#include "datetimemodel.h"

namespace dcc {
namespace datetime {

class DatetimeModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DatetimeModule(FrameProxyInterface* frame, QObject* parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    ModuleWidget *moduleWidget();
    const QString name() const;
    void contentPopped(ContentWidget * const w);

public Q_SLOTS:
    void showTimeSettingsPage();
    void setFrameAutoHide(const bool visible);

private:
    ~DatetimeModule();

private:
    DatetimeWork* m_work;
    DatetimeModel* m_model;
    Datetime* m_datetimeWidget;
    DateSettings* m_dateSettings;
};
}
}
#endif // DATETIMEMODULE_H
