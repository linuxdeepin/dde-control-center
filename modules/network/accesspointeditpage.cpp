#include "accesspointeditpage.h"

using namespace dcc::network;

AccessPointEditPage::AccessPointEditPage(const QString &path, QWidget *parent)
    : ContentWidget(parent),

      m_path(path)
{
    setTitle(tr("Settings"));
}

void AccessPointEditPage::onDeviceRemoved()
{
    emit back();
}
