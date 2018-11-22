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

#ifndef MIRRORSWIDGET_H
#define MIRRORSWIDGET_H

#include "widgets/contentwidget.h"
#include "mirroritem.h"
#include "widgets/settingsgroup.h"

#include <types/mirrorinfolist.h>

using namespace dcc;

namespace dcc{
namespace update{

class UpdateModel;

class MirrorsWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit MirrorsWidget(UpdateModel *model, QWidget* parent = 0);

    void setModel(UpdateModel *model);

Q_SIGNALS:
    void requestSetDefaultMirror(const MirrorInfo &mirror);
    void requestTestMirrorSpeed();

private Q_SLOTS:
    void setCurItem(MirrorItem* item);
    void onSpeedInfoAvailable(const QMap<QString, int> &info);
    void testButtonClicked();

    void sortMirrorsBySpeed();

private:
    void setDefaultMirror(const MirrorInfo& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

private:
    enum TestProgress {
        NotStarted,
        Running,
        Done
    };

    MirrorItem* m_curItem;
    MirrorInfo m_defaultMirror;
    TestProgress m_testProgress;

    QPushButton *m_testButton;
    SettingsGroup *m_mirrorListGroup;
    QVBoxLayout *m_layout;
};

}
}
#endif // MIRRORSWIDGET_H
