// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QAbstractButton>

class CommonTextButton : public QAbstractButton
{
public:
    explicit CommonTextButton(QWidget *parent = nullptr, const QString &text = "");
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
};
