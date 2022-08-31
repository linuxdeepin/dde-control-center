// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QWidget>

class QColor;

namespace DCC_NAMESPACE {
namespace personalization {
class RoundColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoundColorWidget(const QColor &color, QWidget *parent = nullptr);
    bool isSelected();
    void setSelected(bool selected);
    static const int EXTRA = 2; //2px extra space to avoid line cutted off
    void setColor(const QColor &color);

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_isSelected;
    QColor m_color;
};
}
}
