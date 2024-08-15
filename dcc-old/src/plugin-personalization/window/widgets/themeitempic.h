//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <DSvgRenderer>

#include <QWidget>

class QSize;

class ThemeItemPic : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeItemPic(QWidget *parent = nullptr);
    bool isSelected();
    void setSelected(bool selected);
    void setPath(const QString &picPath);
    ~ThemeItemPic();

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_isSelected = false;
    DTK_GUI_NAMESPACE::DSvgRenderer *render;
};
