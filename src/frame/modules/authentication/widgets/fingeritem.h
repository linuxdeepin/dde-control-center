// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FINGERITEM_H
#define FINGERITEM_H

#include "widgets/settingsitem.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include <DIconButton>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace authentication {
class FingerItem : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit FingerItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setEditMode(const bool editmode);

private:
    QHBoxLayout *m_layout;
    bool m_editMode;
    DIconButton *m_editBtn;
    DIconButton *m_removeBtn;
    QLabel *m_title;
};
}
}

#endif // FINGERITEM_H
