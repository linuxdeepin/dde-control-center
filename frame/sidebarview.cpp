#include "sidebarview.h"
#include "sidebarmodel.h"

#include <QDebug>
#include <QScrollBar>
#include <QResizeEvent>

SidebarView::SidebarView(QWidget *parent)
    : QListView(parent)
{
    m_tips = new DTipsFrame;

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color:transparent;");

    connect(this, &SidebarView::clicked, [this] (QModelIndex index) {

        const ModuleMetaData meta = index.data(SidebarModel::PluginMetaData).value<ModuleMetaData>();

        SidebarModel *model = qobject_cast<SidebarModel *>(this->model());
        model->setSelectedPluginId(meta.id);

        emit moduleSelected(meta);
    });

    connect(this, &SidebarView::entered, [this] (QModelIndex index) {

        const QSize itemSize = model()->data(model()->index(0, 0), Qt::SizeHintRole).toSize();
        QPoint pos = rectForIndex(index).topLeft();
        pos.setY(pos.y() - verticalOffset() + itemSize.height() / 2);

        QWidget *parent = qobject_cast<QWidget *>(this->parent());
        while (parent)
        {
            pos += parent->pos();
            parent = qobject_cast<QWidget *>(parent->parent());
        }

        m_tips->showTipsTextAt(index.data(Qt::ToolTipRole).toString(), pos);
    });
}

SidebarView::~SidebarView()
{
    m_tips->deleteLater();
}

void SidebarView::leaveEvent(QEvent *)
{
    QMetaObject::invokeMethod(m_tips, "hide", Qt::QueuedConnection);
}

void SidebarView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);

    const QSize itemSize = model()->data(model()->index(0, 0), Qt::SizeHintRole).toSize();
    const int nums = e->size().height() / itemSize.height();

    qDebug() << nums << e->size() << itemSize;

    setMaximumHeight(nums * itemSize.height());
}

