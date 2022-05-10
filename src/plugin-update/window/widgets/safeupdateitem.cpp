#include "safeupdateitem.h"

SafeUpdateItem::SafeUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void SafeUpdateItem::init()
{
    setIcon(":/icons/deepin/builtin/icons/dcc_safe_update.svg");
    setClassifyUpdateType(SecurityUpdate);
    m_controlWidget->setDetailEnable(false);
    m_controlWidget->setShowMoreButtonVisible(false);
    m_controlWidget->setVersionVisible(false);
}

void SafeUpdateItem::setData(UpdateItemInfo *updateItemInfo)
{
    UpdateSettingItem::setData(updateItemInfo);

    m_controlWidget->setDatetimeVisible(false);
}


