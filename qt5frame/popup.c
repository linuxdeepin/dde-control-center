#include "popup.h"
#include <QGuiApplication>

PopupItem::PopupItem(QObject *parent): QQuickItem(parent)
{
    //connect(QGuiApplication, SIGNAL());
}

void PopupItem::setWindow(DOverrideWindow *w)
{

//    if (m_rootWindow) {
//        disconnect(m_rootWindow, SIGNAL(mousePressed(QPointF)), this, SLOT(onParentMousePressed(QPointF)));
//    }
    m_rootWindow = w;
//    connect(m_rootWindow, SIGNAL(mousePressed(QPointF)), this, SLOT(onParentMousePressed(QPointF)));
}

PopupItem::onParentMousePressed(QPointF point)
{
//    QPointF* rootPoint = this->mapToItem(NULL, QPointF(0, 0));
//    QRectF* rect = QRectF(rootPoint, this->width(), this->height());
//    if(!rect->contains(point) && m_parentObject)
//        m_parentObject.setVisible(false);
}

void PopupItem::setParentObject(QQuickWindow *v)
{
    m_parentObject = v;
}
