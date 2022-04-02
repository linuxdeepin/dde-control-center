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

#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "interface/namespace.h"
#include "widgets/settingsgroup.h"
#include "widgets/buttontuple.h"
#include "titlebuttonItem.h"

#include <DAbstractDialog>

#include <QPushButton>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class ShortcutModel;
struct ShortcutInfo;
class KeyboardControl;
class ShortcutItem;
class ShortcutContent  : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit ShortcutContent(ShortcutModel *model, QWidget *parent = nullptr);
    void setBottomTip(ShortcutInfo *conflict);
    void setInfo(ShortcutInfo *info);
    void setShortcut(const QString &shortcut);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *conflict);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestDisableShortcut(ShortcutInfo *info);
    void requestCloseDlg();

public Q_SLOTS:
    void keyEvent(bool press, const QString &shortcut);
    void onReplace();
    void onUpdateKey();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    ShortcutModel *m_model;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_info;
    ShortcutItem *m_shortcutItem;
    ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
    QString m_shortcut;
};
}
#endif // SHORTCUTCONTENT_H
