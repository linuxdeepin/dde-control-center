#include <QDebug>
#include <QHBoxLayout>

#include "imagenamebutton.h"

#include "constants.h"

#include "monitorground.h"
#include "monitor.h"
#include "dbus/monitorinterface.h"
#include "dbus/displayinterface.h"
#include "displaymodeitem.h"

DisplayModeItem * getIconButton(const QString &text){
    DisplayModeItem* button = new DisplayModeItem(false, false);
    button->setText(QObject::tr(text.toLatin1().data()));
    button->setClickCheck(false);
    button->setIconName(text.toLower());
    button->hide();

    return button;
}

MonitorGround::MonitorGround(DisplayInterface * display, QWidget *parent):
    QFrame(parent),
    m_dbusDisplay(display)
{
    setStyleSheet(QString("QFrame { background-color: %1; }").arg(DCC::BgDarkColor.name()));

    setFixedSize(310, 210);

    QHBoxLayout *layout = new QHBoxLayout;

    m_recognize = getIconButton("Recognize");
    m_edit = getIconButton("Edit");
    m_split = getIconButton("Split");

    connect(m_split, &DisplayModeItem::clicked, [this]{
        foreach (Monitor *monitor, m_monitors) {
            MonitorInterface *dbus = monitor->dbusInterface();
            if(dbus->isComposited()){
                m_dbusDisplay->SplitMonitor(dbus->name());
            }
        }

        m_split->hide();
        m_edit->show();
    });

    connect(m_edit, &DisplayModeItem::clicked, [this]{
        foreach (Monitor *monitor, m_monitors) {
            monitor->setDraggable(true);
        }

        m_recognize->hide();
        m_edit->hide();
    });

    layout->addStretch(1);
    layout->addWidget(m_recognize, 0, Qt::AlignBottom);
    layout->addWidget(m_edit, 0, Qt::AlignBottom);
    layout->addWidget(m_split, 0, Qt::AlignBottom);
    layout->addStretch(1);

    setLayout(layout);
}

void MonitorGround::addMonitor(Monitor *monitor)
{
    if(m_monitors.contains(monitor)||!monitor)
        return;

    m_monitors << monitor;
    monitor->setParent(this);
    MonitorInterface* dbus = monitor->dbusInterface();

    connect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise, Qt::DirectConnection);
    connect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove, Qt::DirectConnection);
    connect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::XChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::YChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::WidthChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::HeightChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::RotationChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::updateOpenedCount, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::IsCompositedChanged, this, &MonitorGround::updateOpenedCount, Qt::DirectConnection);

    updateOpenedCount();
    relayout();
}

void MonitorGround::removeMonitor(Monitor *monitor)
{
    m_monitors.removeOne(monitor);

    monitor->setParent(NULL);
    MonitorInterface* dbus = monitor->dbusInterface();

    disconnect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise);
    disconnect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove);
    disconnect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease);
    disconnect(dbus, &MonitorInterface::XChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::YChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::WidthChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::HeightChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::RotationChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::updateOpenedCount);
    disconnect(dbus, &MonitorInterface::IsCompositedChanged, this, &MonitorGround::updateOpenedCount);

    updateOpenedCount();
    relayout();
}

void MonitorGround::clear()
{
    foreach (Monitor *monitor, m_monitors) {
        removeMonitor(monitor);
    }
}

void MonitorGround::beginEdit()
{
    m_editing = true;

    updateOpenedCount();
}

void MonitorGround::endEdit()
{
    foreach (Monitor *monitor, m_monitors) {
        monitor->setDraggable(false);
    }

    m_editing = false;

    updateOpenedCount();
}

void MonitorGround::relayout()
{
    QRect max_rect;

    QList<Monitor*> hide_monitors;

    foreach (Monitor *monitor, m_monitors) {
        monitor->setVisible(monitor->dbusInterface()->opened());

        if(monitor->isHidden()){
            hide_monitors << monitor;
            continue;
        }

        QRect rect = monitor->resolution();
        max_rect = rect.united(max_rect);
    }

    qreal rect_scale = qMin((double)width() / max_rect.width() * 0.9,
                            (double)height() / max_rect.height() * 0.6);

    max_rect.setTop(max_rect.top() * rect_scale);
    max_rect.setBottom(max_rect.bottom() * rect_scale);
    max_rect.setLeft(max_rect.left() * rect_scale);
    max_rect.setRight(max_rect.right() * rect_scale);
    max_rect.moveCenter(this->rect().center());

    foreach (Monitor *monitor, m_monitors) {
        QRect rect = monitor->resolution();

        if(monitor->isHidden()){
            continue;
        }

        foreach (Monitor *tmp, hide_monitors) {
            QRect tmp_rect = tmp->resolution();

            int tmp_int = rect.top() - tmp_rect.top();

            if(tmp_int > 0)
                rect.moveTop(rect.top() - qMin(tmp_int, tmp_rect.height()));

            tmp_int = rect.left() - tmp_rect.left();

            if(tmp_int > 0)
                rect.moveLeft(rect.left() - qMin(tmp_int, tmp_rect.width()));
        }

        rect.setTop(rect.top() * rect_scale);
        rect.setBottom(rect.bottom() * rect_scale);
        rect.setLeft(rect.left() * rect_scale);
        rect.setRight(rect.right() * rect_scale);
        rect.moveTopLeft(max_rect.topLeft() + rect.topLeft());

        monitor->setParentRect(max_rect);
        monitor->setGeometry(rect);
    }
}

void MonitorGround::onMonitorMouseMove()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    foreach (Monitor *brother, m_monitors) {
        if(brother == monitor)
            continue;

        if(brother->geometry().contains(monitor->geometry().center())){
            brother->dragEnter(monitor);
        }else{
            brother->dragLeave();
        }
    }
}

void MonitorGround::onMonitorMouseRelease()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    bool tmp = true;

    foreach (Monitor *brother, m_monitors) {
        if(brother == monitor)
            continue;

        if(brother->geometry().contains(monitor->geometry().center())){
            if(brother->drop(monitor)){
                m_monitors.removeOne(monitor);
                brother->setDraggable(false);
                tmp = false;
                m_dbusDisplay->JoinMonitor(monitor->name(), brother->name());
            }
        }
    }

    if(tmp){
        MonitorInterface *dbus = monitor->dbusInterface();
        double scale = dbus->width() / (double)monitor->width();

        dbus->SetPos((monitor->x() - monitor->parentRect().x()) * scale,
                     (monitor->y() - monitor->parentRect().y()) * scale);
    }
}

void MonitorGround::updateOpenedCount()
{
    int openedMonitorCount = 0;

    foreach (Monitor *monitor, m_monitors) {
        MonitorInterface *dbus = monitor->dbusInterface();
        if(dbus->opened()){
            ++openedMonitorCount;
            if(dbus->isComposited())
                ++openedMonitorCount;
        }
    }

    if(m_editing && openedMonitorCount > 1){
        if(openedMonitorCount > m_dbusDisplay->monitors().count()){
            m_split->show();
            m_edit->hide();
        }else{
            m_edit->show();
            m_split->hide();
        }
    }else{
        m_split->hide();
        m_edit->hide();
    }

    m_recognize->setVisible(openedMonitorCount > 1);
}
