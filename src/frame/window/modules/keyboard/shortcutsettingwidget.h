// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SHORTCUTSETTINGWIDGET_H
#define SHORTCUTSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/keyboard/shortcutmodel.h"

#include <DFloatingButton>
#include <com_deepin_daemon_search.h>

using SearchInter = com::deepin::daemon::Search;

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

class WaylandGrab;

namespace dcc {
namespace keyboard {
class ShortcutModel;
class ShortcutItem;
}

namespace widgets {
class SettingsHead;
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class ShortCutSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShortCutSettingWidget(dcc::keyboard::ShortcutModel *model, QWidget *parent = nullptr);
    void addShortcut(QList<dcc::keyboard::ShortcutInfo *> list, dcc::keyboard::ShortcutModel::InfoType type);
    dcc::widgets::SettingsHead *getHead() const;
    void showCustomShotcut();
protected:
    void modifyStatus(bool status);
    void wheelEvent(QWheelEvent *event) override
    {
        update();
    }

Q_SIGNALS:
    void customShortcut();
    void delShortcutInfo(dcc::keyboard::ShortcutInfo *info);
    void requestDisableShortcut(dcc::keyboard::ShortcutInfo *info);
    void shortcutEditChanged(dcc::keyboard::ShortcutInfo *info);
    void requestUpdateKey(dcc::keyboard::ShortcutInfo *info);
    void requestShowConflict(dcc::keyboard::ShortcutInfo *info, const QString &shortcut);
    void requestSaveShortcut(dcc::keyboard::ShortcutInfo *info);
    void requestReset();
    void requestSearch(const QString &key);

    void changed(const QString &in0, int in1);

public Q_SLOTS:
    void onSearchTextChanged(const QString &text);
    void onCustomAdded(dcc::keyboard::ShortcutInfo *info);
    void onDestroyItem(dcc::keyboard::ShortcutInfo *info);
    void onSearchInfo(dcc::keyboard::ShortcutInfo *info, const QString &key);
    void onSearchStringFinish(const QList<dcc::keyboard::ShortcutInfo *> searchList);
    void prepareSearchKeys();
    void onRemoveItem(const QString &id, int type);
    void onShortcutChanged(dcc::keyboard::ShortcutInfo *info);
    void onKeyEvent(bool press, const QString &shortcut);
    void onResetFinished();

    void onGrab(dcc::keyboard::ShortcutInfo *info);

protected:
    void keyPressEvent(QKeyEvent *ke) override;
    void keyReleaseEvent(QKeyEvent *ke) override;

private:
    QWidget *m_searchWidget;
    QWidget *m_widget;
    QPushButton *m_resetBtn;
    QLineEdit *m_searchInput;
    QString m_searchText;
    QVBoxLayout *m_layout;
    QVBoxLayout *m_searchLayout;
    DFloatingButton *m_addCustomShortcut;

    // 正在恢复默认快捷键，不能用禁用按钮，因为会导致焦点切换，体验不好
    bool m_bIsResting = false;

    dcc::widgets::SettingsHead *m_head;
    dcc::widgets::SettingsHead *m_workspaceHead;
    dcc::widgets::SettingsGroup *m_systemGroup;
    dcc::widgets::SettingsGroup *m_windowGroup;
    dcc::widgets::SettingsGroup *m_workspaceGroup;
    dcc::widgets::SettingsGroup *m_assistiveToolsGroup;
    dcc::widgets::SettingsGroup *m_customGroup;
    dcc::widgets::SettingsGroup *m_searchGroup;
    QMap<QString, dcc::keyboard::ShortcutInfo *> m_searchInfos;
    // 预留，如果用户太快，可以等带用户输入完成后才搜索
    QTimer *m_searchDelayTimer;
    dcc::keyboard::ShortcutModel *m_model;
    QList<dcc::keyboard::ShortcutItem *> m_allList;
    QList<dcc::keyboard::ShortcutItem *> m_systemList;
    QList<dcc::keyboard::ShortcutItem *> m_windowList;
    QList<dcc::keyboard::ShortcutItem *> m_workspaceList;
    QList<dcc::keyboard::ShortcutItem *> m_assistiveToolsList;
    QList<dcc::keyboard::ShortcutItem *> m_customList;
    QStringList m_assistiveToolsIdList;
    QStringList m_workspaceIdList;

    WaylandGrab *m_waylandGrab;
};
}
}
#endif // SHORTCUTSETTINGWIDGET_H
