// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TITLELABEL_H
#define TITLELABEL_H

#include <DLabel>

class TitleLabel : public DTK_NAMESPACE::Widget::DLabel
{
    Q_OBJECT
public:
    TitleLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    TitleLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    bool event(QEvent *e) override;
};

#endif // TITLELABEL_H
