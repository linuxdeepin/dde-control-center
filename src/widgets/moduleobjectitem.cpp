#include "widgets/moduleobjectitem.h"

#include <DStyleOption>
#include <DStyledItemDelegate>

#include <QApplication>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
class ModuleObjectItemPrivate
{
public:
    explicit ModuleObjectItemPrivate(ModuleObjectItem *object)
        : q_ptr(object)
    {
    }
    ~ModuleObjectItemPrivate()
    {
        qDeleteAll(m_rightItems);
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
        if (m_rightItems.size() < minCount) {
            for (int i = minCount - m_rightItems.size(); i > 0; --i) {
                if (index < 0)
                    m_rightItems.prepend(new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), false));
                else
                    m_rightItems.append(new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), false));
            }
        }
        int itemIndex = index < 0 ? m_rightItems.size() + index : index;
        return m_rightItems.at(itemIndex);
    }

public:
    ModuleObjectItem *q_ptr;
    Q_DECLARE_PUBLIC(ModuleObjectItem)
    DViewItemActionList m_rightItems;
    QMap<int, QVariant> m_mapData;
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
    QIcon ico = QIcon::fromTheme(icon);
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
    case Dtk::RightActionListRole:
        d->m_rightItems = value.value<DViewItemActionList>();
        break;
    default:
        d->m_mapData.insert(role, value);
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
    case Dtk::RightActionListRole:
        if (!d->m_rightItems.isEmpty())
            return QVariant::fromValue(d->m_rightItems);
        break;
    default:
        if (d->m_mapData.contains(role)) {
            return d->m_mapData.value(role);
        }
        break;
    }
    return QVariant();
}
