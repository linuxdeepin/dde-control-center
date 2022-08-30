// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef AVATARDEL_H
#define AVATARDEL_H

#include <QWidget>
#include <QPaintEvent>
#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

class AvatarDel : public DBlurEffectWidget
{
    Q_OBJECT
public:
    explicit AvatarDel(QWidget *parent = 0);

Q_SIGNALS:
    void click();

public Q_SLOTS:
};

#endif // AVATARDEL_H
