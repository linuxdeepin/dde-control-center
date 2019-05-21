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

#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include "widgets/contentwidget.h"
#include "shortcutmodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/searchinput.h"
#include "widgets/settingshead.h"
#include <QList>
#include <com_deepin_daemon_search.h>

using namespace dcc::widgets;

using SearchInter=com::deepin::daemon::Search;
class QLineEdit;

namespace dcc {
namespace keyboard{
class ShortcutModel;
class ShortcutWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutWidget(ShortcutModel *model, QWidget *parent = 0);

    void addShortcut(QList<ShortcutInfo*> list, ShortcutModel::InfoType type);

    SettingsHead* getHead();

protected:
    void modifyStatus(bool status);

Q_SIGNALS:
    void customShortcut();
    void delShortcutInfo(ShortcutInfo* info);
    void requestDisableShortcut(ShortcutInfo* info);
    void shortcutEditChanged(ShortcutInfo* info);
    void requestUpdateKey(ShortcutInfo *info);
    void requestShowConflict(ShortcutInfo *info, const QString &shortcut);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestReset();

public Q_SLOTS:
    void onSearchTextChanged(const QString &text);
    void onCustomAdded(ShortcutInfo* info);
    void onDestroyItem(ShortcutInfo *info);
    void onSearchInfo(ShortcutInfo* info, const QString& key);
    void onSearchKeysFinished(QDBusPendingCallWatcher* watch);
    void onSearchStringFinish(QDBusPendingCallWatcher* watch);
    void prepareSearchKeys();
    void onRemoveItem(const QString &id, int type);
    void onShortcutChanged(ShortcutInfo *info);
    void onKeyEvent(bool press, const QString &shortcut);

private:
    QWidget* m_searchWidget;
    QWidget* m_widget;
    QLineEdit* m_searchInput;
    QString m_searchText;
    QVBoxLayout* m_layout;
    QVBoxLayout* m_searchLayout;
    QPushButton* m_addCustom;
    SettingsHead *m_head;
    SettingsGroup* m_systemGroup;
    SettingsGroup* m_windowGroup;
    SettingsGroup* m_workspaceGroup;
    SettingsGroup* m_customGroup;
    SettingsGroup* m_searchGroup;
    QMap<QString, ShortcutInfo*> m_searchInfos;
    SearchInter* m_searchInter;
    // 预留，如果用户太快，可以等带用户输入完成后才搜索
    QTimer* m_searchDelayTimer;
    ShortcutModel *m_model;
    QList<ShortcutItem *> m_allList;
    QList<ShortcutItem *> m_systemList;
    QList<ShortcutItem *> m_windowList;
    QList<ShortcutItem *> m_workspaceList;
    QList<ShortcutItem *> m_customList;
};

}
}
#endif // SHORTCUTWIDGET_H
