#include "titlevalueitem.h"
#include <QHBoxLayout>

namespace dcc{
namespace systeminfo {

TitleValueItem::TitleValueItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout = new QHBoxLayout();

    m_title = new QLabel();
    m_value = new QLabel();
    m_value->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->addWidget(m_title);
    layout->addWidget(m_value);

    setLayout(layout);

//    setFixedHeight(36);
}

void TitleValueItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleValueItem::setValue(const QString &value)
{
    m_value->setText(value);
}

LogoItem::LogoItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_logo = new QLabel();
    m_description = new QLabel();
    m_description->setWordWrap(true);
    m_description->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_logo);
    layout->addWidget(m_description);
    layout->setSpacing(15);
    layout->setContentsMargins(0, 25, 0, 0);

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
