// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H
#define INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H

#include <QLabel>
class QPaintEvent;

namespace installer {

// Displays tooltip text with a sharp corner at middle of bottom edge.
class TooltipPin : public QLabel
{
    Q_OBJECT

public:
    enum ArrowDirection { ArrowDown = 0, ArrowTop, ArrowLeft, ArrowRight };

public:
    explicit TooltipPin(QWidget *parent = nullptr);
    void setArrowDirection(ArrowDirection arrowDirection);

public Q_SLOTS:
    // Show tooltip and set position of pin at |point|.
    void popup(const QPoint &point);

protected:
    void paintEvent(QPaintEvent *event) override;
    ArrowDirection m_arrowDirection;
};

} // namespace installer

#endif // INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H
