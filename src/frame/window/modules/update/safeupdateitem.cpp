#include "safeupdateitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

SafeUpdateItem::SafeUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void SafeUpdateItem::init()
{
    setIcon(":/update/updatev20/dcc_safe_update.svg");
    setClassifyUpdateType(SecurityUpdate);
    m_controlWidget->setDetailEnable(false);
    m_controlWidget->setShowMoreButtonVisible(false);
    m_controlWidget->setVersionVisible(false);
}


