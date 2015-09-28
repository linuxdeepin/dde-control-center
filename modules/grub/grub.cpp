#include <QDebug>
#include <QLabel>

#include "grub.h"
#include "grubwidget.h"

Grub::Grub():
    m_grubWidget(new GrubWidget())
{
    QT_TR_NOOP("Boot Menu");

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);
}

Grub::~Grub()
{
    qDebug() << "~Grub()";
    m_grubWidget->deleteLater();
}

QFrame *Grub::getContent()
{
    return m_grubWidget;
}

