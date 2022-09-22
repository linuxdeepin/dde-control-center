// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETWORKDETAILPAGE_H
#define NETWORKDETAILPAGE_H

#include <widgets/contentwidget.h>

class NetworkDetailPage : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit NetworkDetailPage(QWidget *parent = nullptr);
    ~NetworkDetailPage();

private Q_SLOTS:
    void onUpdateNetworkInfo();

private:
    QVBoxLayout *m_groupsLayout;
};

#endif // NETWORKDETAILPAGE_H
