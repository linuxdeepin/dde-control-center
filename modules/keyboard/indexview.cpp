#include "indexview.h"
#include "indexmodel.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QDebug>

namespace dcc {
namespace keyboard{

IndexView::IndexView(QWidget *parent)
    :QListView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerPixel);
//    setSelectionMode(QAbstractItemView::MultiSelection);
//    setPalette(QPalette(QColor(19, 89, 177)));
    setStyleSheet("border: 0px; background: transparent");
}

void IndexView::onClick(const QString &ch)
{
    IndexModel * model = qobject_cast<IndexModel*>(this->model());
    MetaData md;
    md.setText(ch);

    int index = model->indexOf(md);
    if(index != -1)
    {
        scrollTo(model->index(index,0),QAbstractItemView::PositionAtTop);
    }
}

//void IndexView::paintEvent(QPaintEvent *e)
//{
//    QListView::paintEvent(e);
//    return;
//    QColor col = palette().color(QPalette::Background);
//    QPainter painter(viewport());
//    painter.setBrush(col);
//    QRect rect(0,0,viewport()->rect().width(), 26 );
//    painter.save();
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(238, 238, 238, .2));
//    painter.drawRect(rect);
//    painter.restore();
//    painter.drawText(rect.adjusted(3,0,0,0), Qt::AlignVCenter, m_section);
//}

void IndexView::showEvent(QShowEvent *e)
{
    QVariant var = indexAt(QPoint(5,10)).data();
    MetaData md = var.value<MetaData>();
    if(md.pinyin().count() > 0)
        m_section = md.pinyin().at(0).toUpper();

    QListView::showEvent(e);
}

void IndexView::scrollContentsBy(int dx, int dy)
{
    QVariant var = indexAt(QPoint(5,10)).data();
    MetaData md = var.value<MetaData>();
    if(md.pinyin().count() > 0)
        m_section = md.pinyin().at(0).toUpper();

    QListView::scrollContentsBy(dx,dy);
}

}
}
