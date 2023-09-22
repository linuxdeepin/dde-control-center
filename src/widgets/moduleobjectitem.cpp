//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/moduleobjectitem.h"

#include <DStyleOption>
#include <DStyledItemDelegate>
#include <DIconTheme>

#include <QApplication>

#if DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 0, 0)
#    define USE_DCIICON
#endif

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

namespace DCC_NAMESPACE {
class ModuleObjectItemPrivate
{
public:
    explicit ModuleObjectItemPrivate(ModuleObjectItem *object)
        : q_ptr(object)
        , m_item(new DStandardItem())
    {
    }
    ~ModuleObjectItemPrivate()
    {
        delete m_item;
    }
    void setRightIcon(const QIcon &icon, int index = -1)
    {
        getRightItem(index)->setIcon(icon);
        q_ptr->update();
    }

    void setRightText(const QString &text, int index = -1)
    {
        getRightItem(index)->setText(text);
        q_ptr->update();
    }
    DViewItemAction *getRightItem(int index)
    {
        int minCount = index;
        if (index < 0) {
            minCount = -index;
        }
        DViewItemActionList actions = m_item->actionList(Qt::RightEdge);
        if (actions.size() < minCount) {
            DViewItemActionList newActions;
            for (auto &&action : actions) {
                DViewItemAction *newAction = new DViewItemAction(action->alignment(), action->iconSize(), action->maximumSize(), action->isClickable());
                newAction->setText(action->text());
                newAction->setIcon(action->icon());
#ifdef USE_DCIICON
                newAction->setDciIcon(action->dciIcon());
#endif
                if (static_cast<int>(action->textColorRole()) != -1) {
                    newAction->setTextColorRole(action->textColorRole());
                }
                if (static_cast<int>(action->textColorType()) != -1) {
                    newAction->setTextColorRole(action->textColorType());
                }

                newActions.append(newAction);
            }

            for (int i = minCount - actions.size(); i > 0; --i) {
                if (index < 0)
                    newActions.prepend(new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), false));
                else
                    newActions.append(new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), false));
            }
            m_item->setActionList(Qt::RightEdge, newActions);
        }
        const DViewItemActionList itemActions = m_item->actionList(Qt::RightEdge);
        int itemIndex = index < 0 ? itemActions.size() + index : index;
        return itemActions.at(itemIndex);
    }

public:
    ModuleObjectItem *q_ptr;
    Q_DECLARE_PUBLIC(ModuleObjectItem)
    DStandardItem *m_item;
};
}

ModuleObjectItem::ModuleObjectItem(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(ModuleObjectItem)
{
}

ModuleObjectItem::~ModuleObjectItem()
{
}

void ModuleObjectItem::setRightIcon(DStyle::StandardPixmap st, int index)
{
    setRightIcon(DStyle::standardIcon(qApp->style(), st), index);
}

void ModuleObjectItem::setRightIcon(const QString &icon, int index)
{
    QIcon ico = DIconTheme::findQIcon(icon);
    if (ico.isNull())
        ico = QIcon(icon);

    setRightIcon(ico, index);
}

void ModuleObjectItem::setRightIcon(const QIcon &icon, int index)
{
    Q_D(ModuleObjectItem);
    d->setRightIcon(icon, index);
}

void ModuleObjectItem::setRightText(const QString &text, int index)
{
    Q_D(ModuleObjectItem);
    d->setRightText(text, index);
}

DViewItemAction *ModuleObjectItem::getRightItem(int index)
{
    Q_D(ModuleObjectItem);
    return d->getRightItem(index);
}

void ModuleObjectItem::update()
{
    emit moduleDataChanged();
}

void ModuleObjectItem::setData(int role, const QVariant &value)
{
    Q_D(ModuleObjectItem);
    switch (role) {
    case Qt::DisplayRole:
        setDisplayName(value.toString());
        break;
    case Qt::DecorationRole:
        setIcon(value.value<QIcon>());
        break;
    default:
        d->m_item->setData(value, role);
        break;
    }
    update();
}

QVariant ModuleObjectItem::data(int role) const
{
    Q_D(const ModuleObjectItem);
    switch (role) {
    case Qt::DisplayRole:
        return displayName();
    case Qt::DecorationRole:
        return icon();
    default:
        return d->m_item->data(role);
        break;
    }
    return QVariant();
}
