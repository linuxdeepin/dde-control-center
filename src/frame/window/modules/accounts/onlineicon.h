// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef ACCOUNTS_ONLINE_DELEGATE_H
#define ACCOUNTS_ONLINE_DELEGATE_H

#include "interface/namespace.h"

#include <QWidget>
#include <QGraphicsDropShadowEffect>

namespace DCC_NAMESPACE {
namespace accounts {

class OnlineIcon : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineIcon(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QGraphicsDropShadowEffect *m_shadowEffect;
};

}
}

#endif  // ACCOUNTS_ONLINE_DELEGATE_H
