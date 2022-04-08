/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#ifndef SHORTCUTSETTINGWIDGET_H
#define SHORTCUTSETTINGWIDGET_H

#include "interface/namespace.h"
#include "operation/shortcutmodel.h"

#include <DFloatingButton>

#include <QWidget>

class QLineEdit;
class QPushButton;
class QVBoxLayout;

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {

class ShortcutModel;
class ShortcutItem;

class SettingsHead;
class SettingsGroup;

class ShortCutSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShortCutSettingWidget(ShortcutModel *model, QWidget *parent = nullptr);
    void addShortcut(QList<ShortcutInfo *> list, ShortcutModel::InfoType type);
    SettingsHead *getHead() const;

protected:
    void modifyStatus(bool status);
    void wheelEvent(QWheelEvent *event) override
    {
        update();
    }

Q_SIGNALS:
    void delShortcutInfo(ShortcutInfo *info);
    void requestDisableShortcut(ShortcutInfo *info);
    void shortcutEditChanged(ShortcutInfo *info);
    void requestUpdateKey(ShortcutInfo *info);
    void requestShowConflict(ShortcutInfo *info, const QString &shortcut);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestReset();
    void requestSearch(const QString &key);

public Q_SLOTS:
    void onSearchTextChanged(const QString &text);
    void onCustomAdded(ShortcutInfo *info);
    void onDestroyItem(ShortcutInfo *info);
    void onSearchInfo(ShortcutInfo *info, const QString &key);
    void onSearchStringFinish(const QList<ShortcutInfo *> searchList);
    void prepareSearchKeys();
    void onRemoveItem(const QString &id, int type);
    void onShortcutChanged(ShortcutInfo *info);
    void onKeyEvent(bool press, const QString &shortcut);
    void onResetFinished();

private:
    QWidget *m_searchWidget;
    QWidget *m_widget;
    QPushButton *m_resetBtn;
    QLineEdit *m_searchInput;
    QString m_searchText;
    QVBoxLayout *m_layout;
    QVBoxLayout *m_searchLayout;

    // 正在恢复默认快捷键，不能用禁用按钮，因为会导致焦点切换，体验不好
    bool m_bIsResting = false;

    SettingsHead *m_head;
    SettingsHead *m_workspaceHead;
    SettingsGroup *m_systemGroup;
    SettingsGroup *m_windowGroup;
    SettingsGroup *m_workspaceGroup;
    SettingsGroup *m_assistiveToolsGroup;
    SettingsGroup *m_customGroup;
    SettingsGroup *m_searchGroup;
    QMap<QString, ShortcutInfo *> m_searchInfos;
    // 预留，如果用户太快，可以等带用户输入完成后才搜索
    QTimer *m_searchDelayTimer;
    ShortcutModel *m_model;
    QList<ShortcutItem *> m_allList;
    QList<ShortcutItem *> m_systemList;
    QList<ShortcutItem *> m_windowList;
    QList<ShortcutItem *> m_workspaceList;
    QList<ShortcutItem *> m_assistiveToolsList;
    QList<ShortcutItem *> m_customList;
    QStringList m_assistiveToolsIdList;
    QStringList m_workspaceIdList;
};
}
#endif // SHORTCUTSETTINGWIDGET_H
