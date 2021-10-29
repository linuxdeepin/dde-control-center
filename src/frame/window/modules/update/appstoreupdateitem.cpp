#include "appstoreupdateitem.h"

#include <qdbusreply.h>
#include <QDBusInterface>
#include <QDBusPendingCall>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

AppstoreUpdateItem::AppstoreUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void AppstoreUpdateItem::showMore()
{
    Q_EMIT requestOpenAppStroe();
}

void AppstoreUpdateItem::init()
{
    setIcon(":/update/updatev20/dcc_app_update.svg");
    setClassifyUpdateType(ClassifyUpdateType::AppStoreUpdate);
    m_controlWidget->setDetailLabelVisible(false);
    m_controlWidget->setShowMoreButtomText(tr("View in App Store"));
    m_controlWidget->setVersionVisible(false);
}

