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
