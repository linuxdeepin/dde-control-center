#include "summaryitem.h"
#include <QVBoxLayout>

CheckUpdateItem::CheckUpdateItem(QFrame *parent)
    :SettingsItem(parent)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_indicator = new DLoadingIndicator();
    m_indicator->setSmooth(true);
    QPixmap pix(QSize(32,32));
    pix.fill(Qt::red);
    m_indicator->setImageSource(pix);

    layout->addWidget(m_indicator);
    layout->setAlignment(m_indicator, Qt::AlignCenter);
    m_indicator->setLoading(true);

    m_title = new QLabel();
    m_title->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_title);

    setLayout(layout);
    setFixedHeight(100);
}

void CheckUpdateItem::setIndicatorVisible(bool visible)
{
    m_indicator->setVisible(visible);
}

void CheckUpdateItem::setImageSource(const QString &src)
{
    m_indicator->setImageSource(QPixmap(src));
}

void CheckUpdateItem::setLoading(bool value)
{
    m_indicator->setLoading(value);
}

void CheckUpdateItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

SummaryItem::SummaryItem(QFrame *parent)
    :SettingsItem(parent)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_title = new QLabel();
    m_title->setAlignment(Qt::AlignCenter);
    m_details = new QLabel();
    m_details->hide();

    layout->addWidget(m_title);
    layout->addWidget(m_details);

    setLayout(layout);

    setFixedHeight(64);
}

void SummaryItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SummaryItem::setDetails(const QString &details)
{
    m_title->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
    m_details->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_details->setText(details);
    m_details->show();
}
