#include "notificationmanager.h"


Q_GLOBAL_STATIC(Notificationmanager, NotifiManager)

const QString EXCLAMATIONPATH = ":/images/exclamation_24px.svg";

Notificationmanager::Notificationmanager(QObject *parent) : QObject(parent)
{
    windowPosition = QPoint();
}

Notificationmanager *Notificationmanager::instance()
{
    return NotifiManager;
}

void Notificationmanager::showToast(QWidget *parent, ErrorType type)
{
    m_message = new CustomFloatingMessage(CustomFloatingMessage::TransientType, parent);
    m_message->setIcon(EXCLAMATIONPATH);

    if( ErrorType::SystemError == type){
        m_message->setMessage(QObject::tr("System error"));
    }else if( ErrorType::NetworkError == type ){
        m_message->setMessage(QObject::tr("Network error"));
    }else if( ErrorType::ConnectionError == type ){
        m_message->setMessage(QObject::tr("Connection error"));
    }else if( ErrorType::ConnectionTimeout == type ){
        m_message->setMessage(QObject::tr("Connection timeout"));
    }else {
        m_message->setMessage(QObject::tr("Network error, connection timed out"));
    }
    m_message->setDuration(2000);
    m_message->adjustSize();
    m_message->move((parent->width() - m_message->width()) / 2, (parent->height() - m_message->height()) / 2);
    m_message->show();
}

void Notificationmanager::setWindowPosition(QPoint pos)
{
    windowPosition = pos;
}

QPoint Notificationmanager::getWindowPosition() const
{
    return windowPosition;
}
