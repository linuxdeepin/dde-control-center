// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef HORIZONTALSEPARATOR_H
#define HORIZONTALSEPARATOR_H

#include <QWidget>

class HorizontalSeparator : public QWidget
{
    Q_OBJECT

public:
    explicit HorizontalSeparator(QWidget *parent = nullptr);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // HORIZONTALSEPARATOR_H
