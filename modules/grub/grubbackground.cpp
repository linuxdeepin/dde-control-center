#include "grubbackground.h"
#include "grubdbus.h"
#include <QPainter>

GrubBackground::GrubBackground(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    GrubDbus *dbus = GrubDbus::instance();
    updateBackground(dbus->background());
    connect(dbus, &GrubDbus::backgroundChanged, this, &GrubBackground::updateBackground);
}

void GrubBackground::paintEvent(QPaintEvent *e)
{
    QPainter pa(this);
    pa.drawPixmap(rect(), m_background);

    QWidget::paintEvent(e);
}

bool GrubBackground::updateBackground(const QString &filename)
{
    bool result = m_background.load(filename);
    if(result){
        setMinimumSize(m_background.size());
        update();
    }
    return result;
}

