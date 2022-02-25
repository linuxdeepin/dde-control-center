/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "window/utils.h"

#include <QModelIndex>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QStandardItem;
class QStandardItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace authentication {
class LoginOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginOptionsWidget(QWidget *parent = nullptr);
    virtual ~LoginOptionsWidget();

    int showPath(const QString &path);
    void showDefaultWidget();

    void setFingerVisible(const bool visible);
    void setFaceIdVisible(const bool visible);
    void setIrisVisible(const bool visible);

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);

private:
    void initUI();
    void initMembers();
    void initConnections();
    bool configContent(const QString & configName);
    void setItemVisible(const int row, const bool visible);

Q_SIGNALS:
    void requestShowFingerDetail();
    void requestShowFaceIdDetail();
    void requestShowIrisDetail();
    void requsetDeviceManagesPage();
    void requestUpdateSecondMenu(bool);

private:
    QList <ListSubItem> m_itemList;
    dcc::widgets::MultiSelectListView *m_deviceListView;
    QStandardItemModel *m_deviceItemModel;
    QModelIndex m_currentIndex;
};

}   // namespace authentication
}   // namespace dccV20
