/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "interface/namespace.h"

#include <QWidget>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace bluetooth {
class TitleEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TitleEdit(QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetBluetoothName(const QString &name);

public Q_SLOTS:
    void setName();
    void setEdit();
    void setTitle(const QString &title);

private:
    QLabel *m_name;
    DLineEdit *m_lineEdit;
};
}
}
