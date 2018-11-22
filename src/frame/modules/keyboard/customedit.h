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

#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/lineeditwidget.h"
#include "shortcutitem.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include <QObject>

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
struct ShortcutInfo;
class CustomItem;
class CustomEdit : public ContentWidget
{
    Q_OBJECT
public:
    explicit CustomEdit(ShortcutModel *model, QWidget *parent = 0);
    void setShortcut(ShortcutInfo* info);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *info);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void setBottomTip(ShortcutInfo *conflict);
    void keyEvent(bool press, const QString &shortcut);

private Q_SLOTS:
    void onOpenFile();
    void onSaveAccels();
    void onUpdateKey();

private:
    ShortcutModel *m_model;
    SettingsGroup  *m_commandGroup;
    LineEditWidget *m_name;
    LineEditWidget *m_command;
    CustomItem   *m_short;
    ShortcutInfo   *m_info;
    QLabel         *m_tip;
    ShortcutInfo *m_conflict;
};
}
}

#endif // CUSTOMEDIT_H
