#include <QIcon>
#include <QLabel>

#include <dde-dock/dockconstants.h>

#include "compositecomponent.h"

CompositeComponent::CompositeComponent(QString id, QObject *parent) :
    QObject(parent),
    m_id(id)
{
    QIcon::setThemeName("Deepin");

    m_item = new QLabel;
    m_item->setFixedSize(Dock::APPLET_FASHION_ICON_SIZE, Dock::APPLET_FASHION_ICON_SIZE);
    m_item->setPixmap(QIcon::fromTheme("network-online").pixmap(m_item->size()));
}

QString CompositeComponent::getName()
{
    return "Network";
}

QString CompositeComponent::getTitle()
{
    return getName();
}

QString CompositeComponent::getCommand()
{
    return "dde-control-center network";
}

QWidget * CompositeComponent::getItem()
{
    return m_item;
}

void CompositeComponent::retainItem()
{
    m_item->setParent(NULL);
}

QWidget * CompositeComponent::getApplet()
{
    return NULL;
}

QString CompositeComponent::getMenuContent()
{
    return "";
}

void CompositeComponent::invokeMenuItem(QString, bool)
{

}
