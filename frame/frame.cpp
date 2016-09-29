#include "frame.h"

#include <QScreen>
#include <QApplication>

Frame::Frame(QWidget *parent)
    : QMainWindow(parent),

      m_mainWidget(nullptr)
{
    setFixedSize(300, 900);
    move(qApp->primaryScreen()->geometry().center() - rect().center());

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::init()
{
    m_mainWidget = new MainWidget(this);
    m_mainWidget->setVisible(true);
}
