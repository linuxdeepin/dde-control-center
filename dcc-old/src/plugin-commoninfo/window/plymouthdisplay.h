//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <QObject>
#include <QWidget>
#include <QPixmap>

namespace DCC_NAMESPACE {
class PlyMouthDisplayItem : public QWidget
{
    Q_OBJECT
public:
    explicit PlyMouthDisplayItem(QWidget *parent = nullptr);

public slots:
    void setLogoPixmap(const QPixmap &pix);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_logo;
};
} // namespace DCC_NAMESPACE
