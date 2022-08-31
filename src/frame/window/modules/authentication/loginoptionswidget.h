// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
