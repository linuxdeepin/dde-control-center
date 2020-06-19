/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sunkang <sunkang_cm@deepin.com>
 *
 * Maintainer: sunkang <sunkang_cm@deepin.com>
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
#include "developermodedialog.h"

#include <QWidget>
#include <DLabel>

QT_BEGIN_NAMESPACE
class QPushButton;
class QDBusInterface;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SwitchWidget;
}
}

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonInfoModel;

class DeveloperModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeveloperModeWidget(QWidget *parent = nullptr);
    void setModel(CommonInfoModel *model);

Q_SIGNALS:
    void enableDeveloperMode(bool enabled);
    void requestLogin();

private Q_SLOTS:
    void onLoginChanged();

public Q_SLOTS:
    void updateDeveloperModeState(const bool state);

private:
    bool m_enterDev{false};
    QPushButton *m_devBtn;
    CommonInfoModel *m_model{nullptr};    
    QDBusInterface *m_inter;
    DTK_NAMESPACE::Widget::DLabel *m_lab;
    DTK_NAMESPACE::Widget::DLabel *m_dtip;
};
}
}
