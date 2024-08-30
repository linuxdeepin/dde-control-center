//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCOUNTSMODEL_H
#define ACCOUNTSMODEL_H
#include "interface/namespace.h"

#include <DStyledItemDelegate>

#include <QAbstractItemModel>
#include <QGraphicsDropShadowEffect>

namespace DCC_NAMESPACE {
class User;
class UserModel;
}


class AccountsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AccountsModel(QObject *parent = nullptr);
    ~AccountsModel() {}

    void setUserModel(DCC_NAMESPACE::UserModel *userModel);
    DCC_NAMESPACE::User *getUser(const QModelIndex &index) const;
    QModelIndex index(DCC_NAMESPACE::User *user) const;
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private Q_SLOTS:
    void onUserAdded(DCC_NAMESPACE::User *user);
    void onUserRemoved(DCC_NAMESPACE::User *user);
    void onDataChanged();

private:
    QList<DCC_NAMESPACE::User *> m_data;
    DCC_NAMESPACE::UserModel *m_userModel;
};


class UserDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    explicit UserDelegate(QAbstractItemView *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
//    void drawEllipse(QPainter *painter, const QStyleOptionViewItem &option, const int message) const;
//    void drawFocus(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

    void drawOnlineIcon(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;

private:
//    QGraphicsDropShadowEffect *m_shadowEffect;
};

#endif // ACCOUNTSMODEL_H
