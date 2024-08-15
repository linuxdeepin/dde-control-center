//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TITLEBUTTONITEM_H
#define TITLEBUTTONITEM_H

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class TitleButtonItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit TitleButtonItem(QFrame* parent = nullptr);
    void setTitle(const QString& title);
    void setValue(const QString& value);

Q_SIGNALS:
    void click();

private:
    void updateTitleSize();

private:
    QLabel *m_title;
    QPushButton *m_button;
};

#endif // TITLEBUTTONITEM_H
