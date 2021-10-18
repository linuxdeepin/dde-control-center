#include "systemupdateitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

SystemUpdateItem::SystemUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
    , m_line(new DHorizontalLine(this))
{
    setIcon(":/update/updatev20/dcc_system_update.svg");
    setClassifyUpdateType(SystemUpdate);

    m_line->setForegroundRole(DPalette::AlternateBase);
    m_settingsGroup->insertWidget(m_line);
    m_line->setVisible(false);

    if (m_updateDetailItemList.count() > 0) {
        for (DetailInfoItem *item : m_updateDetailItemList) {
            m_settingsGroup->appendItem(item);
        }
    }
}

void SystemUpdateItem::showMore()
{
    for (int i = 0; i < m_updateDetailItemList.count(); i++) {
        m_updateDetailItemList.at(i)->setVisible(true);
        m_line->setVisible(true);
    }

}

void SystemUpdateItem::setData(UpdateItemInfo *updateItemInfo)
{
    UpdateSettingItem::setData(updateItemInfo);

    QList<DetailInfo> detailInfoList = updateItemInfo->detailInfos();
    if (detailInfoList.count() < 1) {
        return;
    }

    if (m_updateDetailItemList.count() > 0) {
        for (DetailInfoItem *item : m_updateDetailItemList) {
            m_settingsGroup->removeItem(item);
        }

        m_updateDetailItemList.clear();
    }

    for (int i = 0; i < detailInfoList.count(); i++) {
        DetailInfoItem *detailInfoItem = new DetailInfoItem(this);
        DetailInfo item = detailInfoList.at(i);
        detailInfoItem->setTitle(item.name);
        detailInfoItem->setDate(item.updateTime);
        detailInfoItem->setLinkData(item.link);
        detailInfoItem->setDetailData(item.info);
        detailInfoItem->setVisible(false);
        m_updateDetailItemList.append(detailInfoItem);
        m_settingsGroup->appendItem(detailInfoItem);
    }
}
