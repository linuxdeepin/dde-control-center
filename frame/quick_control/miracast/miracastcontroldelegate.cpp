#include "miracastcontroldelegate.h"
#include "miracastcontrolmodel.h"

#include <QPainter>

miracastControlDelegate::miracastControlDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void miracastControlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const MiracastInfo info = index.data(MiracastControlModel::MiracastItemInfoRole).value<MiracastInfo>();
    const bool isHoverd = index.data(MiracastControlModel::MiracastItemHoverRole).toBool();
    const bool isNext = index.data(MiracastControlModel::MiracastItemNextRole).toBool();
    const bool isHeader = !info.m_sink && info.m_link;

    painter->setRenderHints(QPainter::Antialiasing);

    if (isHoverd && !isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.15 * 255));

    if (index.row())
    {
        if (!isNext && !isHoverd) {
            painter->setPen(QColor(255, 255, 255, 255 * 0.05));
            painter->drawLine(QPoint(10, option.rect.top()), QPoint(option.rect.right() - 10, option.rect.top()));
        }
    }
    else
    {
        painter->setPen(QColor(255, 255, 255, 255 * 0.1));
        painter->drawLine(QPoint(0, option.rect.top()), QPoint(option.rect.right(), option.rect.top()));
    }

    painter->setPen(Qt::white);

    QFont f(painter->font());
    f.setWeight(isHeader ? 500 : 300);
    painter->setFont(f);

    const QRect r = option.rect.marginsRemoved(QMargins(24, 0, 0, 0));

    const int x = option.rect.right() - 24;
    const int y = option.rect.top() + (option.rect.height() - 16) / 2;

    painter->drawText(r, Qt::AlignLeft | Qt::AlignVCenter, index.data(MiracastControlModel::MiracastDisplayRole).toString());
    if (info.m_sink) {
        const bool connectState = index.data(MiracastControlModel::MiracastItemConnectRole).toBool();
        if (connectState) {
            if (isHoverd)
                painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/disconnect.png"));
            else
                painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));
        }
    } else
        painter->drawText(QRect(option.rect.left(), option.rect.top(), option.rect.width() - 10, option.rect.height()), Qt::AlignVCenter | Qt::AlignRight, info.m_link->m_managed ? tr("Active") : tr("Inactive"));
}

QSize miracastControlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
