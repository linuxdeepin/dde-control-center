#include "miracastitem.h"
#include <QHBoxLayout>

using namespace dcc;
using namespace dcc::display;
using namespace dcc::widgets;

MiracastItem::MiracastItem(const SinkInfo &info, QWidget *parent) :
    SettingsItem(parent),
    m_info(info)
{
    QHBoxLayout *mainlayout = new QHBoxLayout;

    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(10, 0, 10, 0);

    QLabel *title = new QLabel(m_info.m_name);
    mainlayout->addWidget(title);

    setLayout(mainlayout);
}

const SinkInfo MiracastItem::info() const
{
   return m_info;
}

void MiracastItem::onConnectState(bool state)
{
    m_info.m_connected = state;
}

void MiracastItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (!m_info.m_connected)
        emit requestSinkConnect(m_info.m_sinkPath, QRect(0, 0, 1920, 1080));
}
