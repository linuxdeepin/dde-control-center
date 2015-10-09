#include <QDebug>
#include <QLabel>

#include "mainwidget.h"
#include "shortcuts.h"

Shortcuts::Shortcuts():
    m_mainWidget(new MainWidget())
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Keyboard Shortcuts"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);
}

Shortcuts::~Shortcuts()
{
    qDebug() << "~Shortcuts()";
    m_mainWidget->setParent(0);
    m_mainWidget->deleteLater();
}

QFrame *Shortcuts::getContent()
{
    return m_mainWidget;
}

