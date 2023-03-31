//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QWidget>
#include <QColor>
#include <QPainterPath>

namespace DCC_NAMESPACE {

class AvatarCropBox : public QWidget
{
    Q_OBJECT
public:
    AvatarCropBox(QWidget *parent=nullptr);
    ~AvatarCropBox();

    void setBackgroundColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_backgroundColor;
};

} // DCC_NAMESPACE
