//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FINGERITEM_H
#define FINGERITEM_H

#include <dtkwidget_global.h>
#include "widgets/settingsitem.h"
#include <QWidget>

#include <DIconButton>

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QLabel;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class FingerItem : public DCC_NAMESPACE::SettingsItem
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

#endif // FINGERITEM_H
