// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DIconButton>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QHBoxLayout;
QT_END_NAMESPACE

class KeyLabel;

namespace DCC_NAMESPACE {
namespace keyboard {
class CheckItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CheckItem(QFrame *parent = nullptr);
    void setTitle(const QString &title);
    QString title() const;

    void setKeyLabelEffect(const QStringList &shortCutKeys);

    void setChecked(bool checked);
    void setMultipleMode(bool multiple);
    bool checked() const;

Q_SIGNALS:
    void checkedChanged(const QString &title);
    void destroySelf(CheckItem *item);

public Q_SLOTS:
    void onEditMode(bool value);
    void onClick();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QHBoxLayout *m_hLayout;
    QList<KeyLabel *> m_list;
    bool m_checked;
    bool m_bEdit;
    QLabel *m_title;
    DTK_WIDGET_NAMESPACE::DIconButton *m_checkBtn;
    int m_circle;
    bool m_multiple;
};
}
}
