#include "connectwidget.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDebug>

namespace dcc {
namespace widgets {

ConnectWidget::ConnectWidget(QWidget *parent) :
    QFrame(parent),
    m_connectStateBtn(new DImageButton),
    m_loading(new LoadingIndicator)
{
    m_loading->setVisible(false);
    m_loading->setFixedSize(22, 22);

    m_connectStateBtn->setVisible(false);
    m_connectStateBtn->setNormalPic(":/display/themes/dark/icons/select.png");
    m_connectStateBtn->setHoverPic(":/display/themes/dark/icons/select.png");
    m_connectStateBtn->setPressPic(":/display/themes/dark/icons/select.png");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_loading, 0, Qt::AlignRight);
    mainLayout->addWidget(m_connectStateBtn, 0, Qt::AlignRight);

    setLayout(mainLayout);

    connect(m_connectStateBtn, &DImageButton::clicked, this, &ConnectWidget::clicked);
}

void ConnectWidget::onConnectChanged(ConnectState state)
{
    m_state = state;

    switch (state) {
    case Connected:
        m_loading->stop();
        m_loading->setVisible(false);
        m_connectStateBtn->setVisible(true);
        break;
    case ConnectFaild:
        m_loading->stop();
        m_loading->setVisible(false);
        m_connectStateBtn->setVisible(false);
        break;
    case Connecting:
        m_loading->play();
        m_loading->setVisible(true);
        m_connectStateBtn->setVisible(false);
        break;
    default:
        break;
    }
}

void ConnectWidget::enterEvent(QEvent *event)
{
    QFrame::enterEvent(event);

    if (m_state == Connected) {
        m_connectStateBtn->setNormalPic(":/display/themes/dark/icons/list_delete_normal.png");
        m_connectStateBtn->setHoverPic(":/display/themes/dark/icons/list_delete_hover.png");
        m_connectStateBtn->setPressPic(":/display/themes/dark/icons/list_delete_press.png");
    }

}

void ConnectWidget::leaveEvent(QEvent *event)
{
    QFrame::leaveEvent(event);

    if (m_state == Connected) {
        m_connectStateBtn->setNormalPic(":/display/themes/dark/icons/select.png");
        m_connectStateBtn->setHoverPic(":/display/themes/dark/icons/select.png");
        m_connectStateBtn->setPressPic(":/display/themes/dark/icons/select.png");
    }
}

}
}
