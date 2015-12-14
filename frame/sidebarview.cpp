#include "sidebarview.h"
#include "sidebarmodel.h"
#include "controlcenterproxy.h"

#include <QDebug>
#include <QScrollBar>
#include <QResizeEvent>

SidebarView::SidebarView(QWidget *parent)
    : QListView(parent)
{
    m_tips = new DTipsFrame;

    // WARNING: if not set Qt::WA_Hover attribute to viewport,
    // item cant receive "mouse leave" event, and items mouse hover state will error
    // TODO: its Qts issue, remove after qt fix it
    viewport()->setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color:transparent;");

    ControlCenterProxy *proxy = ControlCenterProxy::getInstance();
    connect(proxy, &ControlCenterProxy::frameSideChanged, [this] (bool leftSide) {
        m_tips->setArrowDirection(leftSide ? DTipsFrame::ArrowLeft : DTipsFrame::ArrowRight);
    });

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

        m_tips->showTipsTextAt(index.data(SidebarModel::PluginName).toString(), pos);
    });
}

SidebarView::~SidebarView()
{
    m_tips->deleteLater();
}

void SidebarView::leaveEvent(QEvent *e)
{
    QListView::leaveEvent(e);
//    QListView::update(m_lastHoverIndex);

    QMetaObject::invokeMethod(m_tips, "hide", Qt::QueuedConnection);
//    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection, QGenericReturnArgument(), Q_ARG(QModelIndex, m_lastHoverIndex));
}

void SidebarView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);

    const QSize itemSize = model()->data(model()->index(0, 0), Qt::SizeHintRole).toSize();
    const int nums = e->size().height() / itemSize.height();
    setMaximumHeight(nums * itemSize.height());
}

void SidebarView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);

    SidebarModel *sidebar = qobject_cast<SidebarModel *>(model);
    if (!sidebar)
        return;

    connect(sidebar, &SidebarModel::switchToModel, this, &SidebarView::moduleSelected);
}

QSize SidebarView::sizeHint() const
{
    return QSize(24, 48 * model()->rowCount());
}

