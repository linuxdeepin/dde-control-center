/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
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

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class VersionProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VersionProtocolWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void loadTextFinished();

private:
    QVBoxLayout *m_mainLayout;
    QLabel *m_title;
    QLabel *m_body;
};

}
