#include "miracastitem.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>

#include <QDebug>

using namespace dcc;
using namespace dcc::display;
using namespace dcc::widgets;

MiracastItem::MiracastItem(const SinkInfo &info, QWidget *parent) :
    SettingsItem(parent),
    m_info(info)
{
    QLabel *title = new QLabel(m_info.m_name);
    m_connectWidget = new ConnectWidget;

    if (m_info.m_connected)
        m_connectWidget->onConnectChanged(ConnectWidget::Connected);
    else
        m_connectWidget->onConnectChanged(ConnectWidget::ConnectFaild);

    QHBoxLayout *mainlayout = new QHBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(20, 5, 10, 5);
    mainlayout->addWidget(title);
    mainlayout->addStretch();
    mainlayout->addWidget(m_connectWidget, 0, Qt::AlignRight);
    setLayout(mainlayout);
    setFixedHeight(36);

    connect(m_connectWidget, &ConnectWidget::clicked, this, &MiracastItem::onDisConnect);
}

const SinkInfo MiracastItem::info() const
{
   return m_info;
}

void MiracastItem::onConnectState(bool state)
{
    m_info.m_connected = state;

    if (state)
        m_connectWidget->onConnectChanged(ConnectWidget::Connected);
    else
        m_connectWidget->onConnectChanged(ConnectWidget::ConnectFaild);
}

void MiracastItem::onDisConnect()
{
    emit requestSinkDisConnect(m_info.m_sinkPath);
}

void MiracastItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (!m_info.m_connected) {
        m_connectWidget->onConnectChanged(ConnectWidget::Connecting);
        emit requestSinkConnect(m_info.m_sinkPath, QApplication::primaryScreen()->availableGeometry());
    }
}

void MiracastItem::enterEvent(QEvent *e)
{
    SettingsItem::enterEvent(e);

    m_connectWidget->setMouseEnter(true);
}

void MiracastItem::leaveEvent(QEvent *e)
{
    SettingsItem::leaveEvent(e);

    m_connectWidget->setMouseEnter(false);
}
