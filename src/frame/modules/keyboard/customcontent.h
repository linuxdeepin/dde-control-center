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

#ifndef CUSTOMCONTENT_H
#define CUSTOMCONTENT_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "keyboardcontrol.h"
#include <QPushButton>

#include "widgets/buttontuple.h"
#include "widgets/lineeditwidget.h"
#include "customitem.h"

using namespace dcc;

namespace dcc {
namespace keyboard{

class KeyboardWorker;
struct ShortcutInfo;

class CustomContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomContent(ShortcutModel *model, QWidget *parent = 0);

    void setBottomTip(ShortcutInfo *conflict);

Q_SIGNALS:
    void requestAddKey(const QString& name, const QString& command, const QString& accels);
    void requestUpdateKey(ShortcutInfo *info);
    void requestForceSubs(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void onShortcut();
    void keyEvent(bool press, const QString &shortcut);
    void updateKey();

private Q_SLOTS:
    void onOpenFile();

private:
    KeyboardWorker* m_work;
    SettingsGroup *m_commandGroup;
    LineEditWidget* m_name;
    LineEditWidget* m_command;
    CustomItem *m_shortcut;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
    ShortcutModel *m_model;
    dcc::widgets::ButtonTuple *m_buttonTuple;
};
}
}
#endif // CUSTOMCONTENT_H
