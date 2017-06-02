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
    m_connect = new QLabel(tr("Connected"));
    m_connect->setVisible(m_info.m_connected);
    m_loading = new LoadingIndicator;
    m_loading->setVisible(false);

    QHBoxLayout *mainlayout = new QHBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(20, 10, 10, 10);
    mainlayout->addWidget(title);
    mainlayout->addStretch();
    mainlayout->addWidget(m_connect);
    mainlayout->addWidget(m_loading);
    setLayout(mainlayout);
    setFixedHeight(36);
}

const SinkInfo MiracastItem::info() const
{
   return m_info;
}

void MiracastItem::onConnectState(bool state)
{
    m_info.m_connected = state;

    m_connect->setVisible(state);
    m_loading->setVisible(false);
}

void MiracastItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (!m_info.m_connected) {
        emit requestSinkConnect(m_info.m_sinkPath, QApplication::primaryScreen()->availableGeometry());
        m_loading->setVisible(true);
        m_loading->play();
    } else {
        emit requestSinkDisConnect(m_info.m_sinkPath);
    }
}
