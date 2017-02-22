#include "logoitem.h"
#include <QHBoxLayout>

#include "labels/tipslabel.h"

namespace dcc{
namespace systeminfo {

LogoItem::LogoItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_logo = new QLabel;
    m_description = new TipsLabel;
    m_description->setWordWrap(true);
    m_description->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_logo);
    layout->addWidget(m_description);
    layout->setSpacing(15);
    layout->setContentsMargins(0, 25, 0, 20);

    layout->setAlignment(m_logo, Qt::AlignCenter);
    layout->setAlignment(m_description, Qt::AlignCenter);
    setLayout(layout);
}


void LogoItem::setDescription(const QString &des)
{
    m_description->setText(des);
}

void LogoItem::setLogo(const QString &logo)
{
    m_logo->setPixmap(QPixmap(logo));
}

}
}
