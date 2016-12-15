#include "accesspointeditpage.h"

using namespace dcc::network;

AccessPointEditPage::AccessPointEditPage(const QString &path, QWidget *parent)
    : ContentWidget(parent),

      m_path(path)
{
#ifdef QT_DEBUG
    setTitle(path);
#else
    setTitle(tr("Settings"));
#endif
}

void AccessPointEditPage::onDeviceRemoved()
{
    emit back();
}
