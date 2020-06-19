/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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
#include "loginedin.h"

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DTipLabel;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QStandardItem;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SwitchWidget;
}

namespace unionid {
class UnionidModel;
}
}

namespace DCC_NAMESPACE {
namespace unionid {
class DownloadUrl;
class AvatarWidget;
class IndexPage : public LoginedIn
{
    Q_OBJECT
public:
    IndexPage(QWidget *parent = nullptr);
    void setModel(dcc::unionid::UnionidModel *model);
    ~IndexPage();

Q_SIGNALS:
    void requestSetAutoSync(bool enable) const;
    void requestLogout() const;
    void requesUserDialog(QString) const;

private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::SwitchWidget *m_autoSyncSwitch;
    QLabel *m_stateLbl;
    QStandardItemModel *m_listModel;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_networkTip;
    QLabel *m_logout;
};
} // namespace sync
} // namespace DCC_NAMESPACE
