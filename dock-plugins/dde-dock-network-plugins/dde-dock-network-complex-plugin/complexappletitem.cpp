#include <QHBoxLayout>
#include "complexappletitem.h"
#include <dde-dock/dockconstants.h>

const int ICON_SIZE = 40;
const int INDEX_LABEL_HEIGHT = 10;

ComplexAppletItem::ComplexAppletItem(QWidget *parent)
    : DImageButton(parent)
{
    setAlignment(Qt::AlignCenter);
    setFixedSize(ICON_SIZE, ICON_SIZE);

    m_indexLabel = new QLabel(this);
    m_indexLabel->setObjectName("IndexLabel");
    m_indexLabel->setAlignment(Qt::AlignCenter);
    m_indexLabel->setFixedSize(ICON_SIZE, INDEX_LABEL_HEIGHT);
    m_indexLabel->move(0, ICON_SIZE - INDEX_LABEL_HEIGHT);
}

void ComplexAppletItem::setIcon(const QString &icon)
{
    setNormalPic(icon);
    setHoverPic(icon);
    setPressPic(icon);
}

QString ComplexAppletItem::index() const
{
    return m_indexLabel->text();
}

void ComplexAppletItem::setIndex(const QString &value)
{
    if (value.isEmpty()) {
        m_indexLabel->hide();
    }
    else {
        m_indexLabel->setText(value);
        m_indexLabel->show();
    }
}

ComplexAppletItem::DeviceType ComplexAppletItem::deviceType() const
{
    return m_deviceType;
}

void ComplexAppletItem::setDeviceType(const DeviceType &deviceType)
{
    m_deviceType = deviceType;
}

