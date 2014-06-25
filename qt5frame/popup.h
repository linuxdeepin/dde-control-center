#ifndef POPUP_H
#define POPUP_H

#include <QObject>
#include <QQuickItem>
#include <QPointer>
#include "dwindow.h"

class PopupItem : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(PopupItem)

    Q_PROPERTY(DOverrideWindow* window READ window WRITE setWindow)
    Q_PROPERTY(QQuickItem* parentObject READ parentObject WRITE setParentObject NOTIFY parentObjectChanged)
public:
    PopupItem(QObject *parent = 0);

    DOverrideWindow* window() { return m_rootWindow; }
    void setWindow(DOverrideWindow* w);
    Q_SLOT void onParentMousePressed(QPointF point);

    QQuickItem* parentObject() { return m_parentObject; }
    void setParentObject(QQuickWindow* v);
    Q_SIGNAL void parentObjectChanged(QQuickItem);

private:
    QPointer<DOverrideWindow> m_rootWindow;
    QPointer<QQuickItem> m_parentObject;
}

#endif // POPUP_H
