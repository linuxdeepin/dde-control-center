//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_UPDATE_RESULTITEM_H
#define DCC_UPDATE_RESULTITEM_H

#include "widgets/settingsitem.h"

#include "dimagebutton.h"
#include "common.h"

class ResultItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT

public:
    explicit ResultItem(QFrame* parent = 0);

    void setSuccess(ShowStatus type);
    void setMessage(const QString &message);

private:
    QLabel* m_message;
    QLabel *m_icon;
    QString m_pix;
};


#endif // DCC_UPDATE_RESULTITEM_H
