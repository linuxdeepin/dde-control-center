//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "accountsmodel.h"
#include "operation/user.h"
#include "operation/usermodel.h"

#include <DStyleOptionBackgroundGroup>
#include <DStyle>

#include <QIcon>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QPainterPath>

static constexpr int OnlineLeftReserve = 10;
static constexpr int OnineDisplayWidthReserve = OnlineLeftReserve * 2;
static constexpr int DisplayHeight = 15;
static constexpr QSize OnlineSize = QSize(OnlineLeftReserve, OnlineLeftReserve);

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

void AccountsModel::setUserModel(UserModel *userModel)
{
    m_userModel = userModel;
    connect(userModel,&UserModel::userAdded,this,&AccountsModel::onUserAdded);
    connect(userModel,&UserModel::userRemoved,this,&AccountsModel::onUserRemoved);
    for (auto &&user:userModel->userList())
        onUserAdded(user);
}

User *AccountsModel::getUser(const QModelIndex &index) const
{
    int row = index.row();
    if (row < 0 || row >= m_data.size())
        return nullptr;
    return m_data.at(row);
}

QModelIndex AccountsModel::index(User *user) const
{
    return index(m_data.indexOf(user), 0);
}

QModelIndex AccountsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_data.size())
        return QModelIndex();
    return createIndex(row, column, m_data.at(row));
}

QModelIndex AccountsModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_data.size();

    return 0;
}

int AccountsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    if (m_data.isEmpty() || !index.isValid())
        return QVariant();

    int row = index.row();
    User *user = m_data.at(row);
    switch (role) {
    case Qt::DisplayRole:
        return user->fullname().isEmpty()?user->name():user->fullname();
    case Qt::DecorationRole:
        return QIcon(user->currentAvatar().mid(7));
    case Qt::CheckStateRole:
        return user->online()?Qt::Checked:Qt::Unchecked;
    default:
        break;
    }
    return QVariant();
}

void AccountsModel::onUserAdded(User *user)
{
    int row = user->isCurrentUser() ? 0 : m_data.size();
    connect(user,&User::nameChanged,this,&AccountsModel::onDataChanged);
    connect(user,&User::fullnameChanged,this,&AccountsModel::onDataChanged);
    connect(user,&User::currentAvatarChanged,this,&AccountsModel::onDataChanged);
    connect(user,&User::onlineChanged,this,&AccountsModel::onDataChanged);

    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row, user);
    endInsertRows();
}

void AccountsModel::onUserRemoved(User *user)
{
    int row = m_data.indexOf(user);
    if (row >= 0 && row < m_data.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_data.removeAt(row);
        endRemoveRows();
    }
}

void AccountsModel::onDataChanged()
{
    User *user = qobject_cast<User *>(sender());
    if (user) {
        QModelIndex i = index(m_data.indexOf(user),0);
        emit dataChanged(i,i);
    }
}

UserDelegate::UserDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
}

void UserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    // 选择高亮背景
    if (opt.state & QStyle::State_Selected) {
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                                          ? QPalette::Normal
                                          : QPalette::Disabled;
        opt.backgroundBrush = option.palette.color(cg, QPalette::Highlight);
    }
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    QRect decorationRect;
    decorationRect = QRect(opt.rect.topLeft() + QPoint((opt.rect.width() - opt.decorationSize.width()) / 2, 3), opt.decorationSize);

    opt.displayAlignment = Qt::AlignLeft|Qt::AlignVCenter;

    // draw the item
    drawBackground(style, painter, opt, decorationRect);
    // 图标的绘制用也可能会使用这些颜色
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
    painter->setPen(opt.palette.color(cg, QPalette::Text));//(opt.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text));
    drawDecoration(painter, opt, decorationRect);

    opt.displayAlignment = Qt::AlignCenter;

    bool hasChecked = index.data(Qt::CheckStateRole) == Qt::Checked;

    QRect displayRect = QRect(opt.rect.topLeft() + QPoint(0, opt.decorationSize.height() + 4), QSize(opt.rect.width(), DisplayHeight));

    if (hasChecked) {
        displayRect = QRect(opt.rect.topLeft() + QPoint(OnlineLeftReserve, opt.decorationSize.height() + 4), QSize(opt.rect.width() - OnineDisplayWidthReserve, DisplayHeight));
        QRect onlineRect = QRect(opt.rect.topLeft() + QPoint(0, opt.decorationSize.height() + 8), OnlineSize);
        drawOnlineIcon(painter, opt, onlineRect);
    }

    drawDisplay(style, painter, opt, displayRect);
    painter->restore();
}

QSize UserDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return QSize(60,60);
}

void UserDelegate::drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(style)
    QRect r = rect;
    r.adjust(-2,-2,2,2);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(option.palette.color(QPalette::Normal,(option.state & QStyle::State_Selected)? QPalette::Highlight:QPalette::Window));
    painter->drawRoundedRect(r,8,8);
    painter->restore();
}

void UserDelegate::drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    DStyle::viewItemDrawText(style, painter, &option, rect);
}


void UserDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if (option.features & QStyleOptionViewItem::HasDecoration) {
        QIcon::Mode mode = QIcon::Normal;
        if (!(option.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if (option.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        QIcon::State state = (option.state & QStyle::State_Open) ? QIcon::On : QIcon::Off;
        painter->save();
        QPainterPath painterPath;
        painterPath.addRoundedRect(rect,8,8);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setClipPath(painterPath);
        option.icon.paint(painter, rect, option.decorationAlignment, mode, state);

        painter->restore();
    }
}


void UserDelegate::drawOnlineIcon(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    Q_UNUSED(option)
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);

    painter->setBrush(QBrush(QColor(103, 239, 74)));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(rect.adjusted(1, 1, -1, -1));

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(rect.adjusted(1, 1, -1, -1));
    painter->restore();
}
