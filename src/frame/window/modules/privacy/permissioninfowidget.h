// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PERMISSIONINFOWIDGET_H
#define PERMISSIONINFOWIDGET_H

#include <QAbstractItemModel>
#include <QWidget>

namespace Dtk {
namespace Widget {
class DSwitchButton;
}
}
struct PrivacyPage;
class PrivacySecurityModel;
class QTreeView;
class ApplicationItem;

class AppPermissionItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AppPermissionItemModel(PrivacySecurityModel *model, QList<int> premission, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const override;
    QModelIndex index(ApplicationItem *item) const;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    PrivacySecurityModel *m_model;
    QList<int> m_premission;
};

class PermissionInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PermissionInfoWidget(const PrivacyPage &page, PrivacySecurityModel *model, QWidget *parent = nullptr);
    ~PermissionInfoWidget();

private Q_SLOTS:
    void mainPremissionEnabledChanged(int premission, bool enabled);
    void setMainPremissionEnabled(bool enabled);
    void onAppSizeChanged();

private:
    void initUI();
    void initConnect();

private:
    const PrivacyPage &m_privacyPage;
    PrivacySecurityModel *m_model;
    Dtk::Widget::DSwitchButton *m_btnEnabled;
    QTreeView *m_appView;
    QWidget *m_noneWidget;
};

#endif // PERMISSIONINFOWIDGET_H
