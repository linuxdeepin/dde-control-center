#include <QLabel>
#include <QDebug>
#include <QTimer>

#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>

#include "moduleheader.h"

ModuleHeader::ModuleHeader(QString title, bool addResetButton, QWidget * parent) :
    DBaseLine(parent),
    m_timer(nullptr)
{
    D_THEME_INIT_WIDGET(ModuleHeader);

    this->setFixedHeight(48);

    m_title = new QLabel(title, this);
    this->setLeftContent(m_title);

    if (addResetButton) {
        m_tipsLabel = new QLabel(tr("Reset Success"), this);
        m_tipsLabel->setObjectName("TipsLabel");
        m_reset = new DTextButton("Reset", this);
        m_timer = new QTimer;

        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(m_tipsLabel);
        layout->addWidget(m_reset);

        QWidget *widget = new QWidget;
        widget->setLayout(layout);

        setRightContent(widget);

        m_timer->setInterval(800);

        connect(m_timer, &QTimer::timeout, this, &ModuleHeader::resetUI);
        connect(m_reset, &DTextButton::clicked, this, &ModuleHeader::resetButtonClicked);
        connect(m_reset, &DTextButton::clicked, [this] {
            m_tipsLabel->show();
            m_reset->hide();
            m_timer->start();
        });

        resetUI();
    }
}

ModuleHeader::~ModuleHeader()
{
    if (m_timer)
        m_timer->deleteLater();
}

void ModuleHeader::resetUI()
{
    m_tipsLabel->hide();
    m_reset->show();
}

