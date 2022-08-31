// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef USEROPTIONITEM_H
#define USEROPTIONITEM_H

#include "widgets/nextpagewidget.h"

class QLabel;

namespace dcc {

namespace accounts {

class UserOptionItem : public widgets::NextPageWidget
{
    Q_OBJECT

public:
    explicit UserOptionItem(QFrame *parent = 0);

public Q_SLOTS:
    void setAvatar(const QString &avatar);

private:
    QLabel *m_avatarLabel;
};

}

}

#endif // USEROPTIONITEM_H
