// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "widgets/settingsitem.h"
#include "shortcutkey.h"

#include <DIconButton>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace dcc::widgets;

namespace dcc {
namespace keyboard {

struct ShortcutInfo;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = nullptr);
    ~ShortcutItem();

    void setShortcutInfo(ShortcutInfo *info);
    inline ShortcutInfo *curInfo() { return m_info; }

    void setChecked(bool checked);
    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);

    QString configName() const;
    void setConfigName(const QString &configName);

Q_SIGNALS:
    void shortcutEditChanged(ShortcutInfo *info);
    void requestUpdateKey(ShortcutInfo *info);
    void requestRemove(ShortcutInfo *info);
    void waylandEditKeyFinshed();

public Q_SLOTS:
    void onEditMode(bool value);
    void onRemoveClick();

private:
    void onShortcutEdit();
    void updateTitleSize();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QLineEdit *m_shortcutEdit;
    QLabel *m_title;
    ShortcutInfo *m_info;
    DTK_WIDGET_NAMESPACE::DIconButton *m_delBtn;
    DTK_WIDGET_NAMESPACE::DIconButton *m_editBtn;
    ShortcutKey *m_key;
    QString m_configName;
};
}
}
#endif // SHORTCUTITEM_H
