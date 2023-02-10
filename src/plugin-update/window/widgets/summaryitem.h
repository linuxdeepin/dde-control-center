//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SUMMARYITEM_H
#define SUMMARYITEM_H

#include "widgets/settingsitem.h"
#include <QLabel>

class SummaryItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT

public:
    explicit SummaryItem(QFrame * parent = 0);

    void setTitle(const QString& title);
    void setDetails(const QString& details);

private:
    QLabel *m_title;
    QLabel *m_details;
};

#endif // SUMMARYITEM_H
