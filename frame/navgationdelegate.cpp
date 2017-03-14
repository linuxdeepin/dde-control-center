#include "navgationdelegate.h"
#include "navgationmodel.h"

#include <QPainter>
#include <QDebug>

namespace dcc {

NavgationDelegate::NavgationDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void NavgationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const NavgationModel::EdgeFlags edgeFlags = index.data(NavgationModel::ItemEdgeRole).value<NavgationModel::EdgeFlags>();
    const int r = 5;
    const int ox = option.rect.topLeft().x();
    const int oy = option.rect.topLeft().y();
    const int w = option.rect.width();
    const int h = option.rect.height();

    QPainterPath path(option.rect.topLeft());
    path.addRect(option.rect);

    if (edgeFlags.testFlag(NavgationModel::Top))
    {
        if (edgeFlags.testFlag(NavgationModel::Left))
        {
            // top-left
            QPainterPath sub(option.rect.topLeft());
            sub.lineTo(ox + r, oy);
            sub.arcTo(ox, oy, r * 2, r * 2, 90, 90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
        else if (edgeFlags.testFlag(NavgationModel::Right))
        {
            // top-right
            QPainterPath sub(QPoint(ox + w, oy));
            sub.lineTo(ox + w - r, oy);
            sub.arcTo(ox + w - r * 2, oy, r * 2, r * 2, 90, -90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
    }
    else if (edgeFlags.testFlag(NavgationModel::Bottom))
    {
        if (edgeFlags.testFlag(NavgationModel::Left))
        {
            // bottom-left
            QPainterPath sub(QPoint(ox, oy + h));
            sub.lineTo(ox, oy + h - r);
            sub.arcTo(ox, oy + h - r * 2, r * 2, r * 2, 180, 90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
        else if (edgeFlags.testFlag(NavgationModel::Right))
        {
            // bottom-right
            QPainterPath sub(QPoint(ox + w, oy + h));
            sub.lineTo(ox + w, oy + h - r);
            sub.arcTo(ox + w - r * 2, oy + h - r * 2, r * 2, r * 2, 0, -90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
    }

    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    if (index.data(NavgationModel::ItemHoveredRole).toBool())
        painter->fillPath(path, QColor::fromRgbF(1, 1, 1, 0.3));
    else
        painter->fillPath(path, QColor::fromRgbF(1, 1, 1, 0.2));

    const QString module = index.data().toString();
    if (module.isEmpty())
        return;

    const QString picFile = QString(":/%1/themes/dark/icons/nav_%2.png").arg(module).arg(module);
    QPixmap pix = QPixmap(picFile);

    QRect pixRect = pix.rect();
    pixRect.moveCenter(option.rect.center());

    painter->drawPixmap(pixRect, pix);
}

QSize NavgationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

}
