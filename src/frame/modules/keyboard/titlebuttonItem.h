// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TITLEBUTTONITEM_H
#define TITLEBUTTONITEM_H

#include "widgets/settingsitem.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace dcc::widgets;

class TitleButtonItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit TitleButtonItem(QFrame* parent = 0);
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
