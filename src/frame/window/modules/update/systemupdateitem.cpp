#include "systemupdateitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

SystemUpdateItem::SystemUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
    , m_line(new DHorizontalLine(this))
    , m_lineWidget(new QWidget)
{
    setIcon(":/update/updatev20/dcc_system_update.svg");
    setClassifyUpdateType(SystemUpdate);

    QVBoxLayout *lineLay = new QVBoxLayout();
    lineLay->setMargin(0);
    lineLay->addSpacing(10);
    lineLay->addWidget(m_line);
    m_lineWidget->setLayout(lineLay);
    m_settingsGroup->insertWidget(m_lineWidget);
    m_lineWidget->setVisible(false);
    if (m_updateDetailItemList.count() > 0) {
        for (DetailInfoItem *item : m_updateDetailItemList) {
            m_settingsGroup->appendItem(item);
        }
    }
}

void SystemUpdateItem::showMore()
{
    m_controlWidget->setShowMoreButtonVisible(false);
    for (int i = 0; i < m_updateDetailItemList.count(); i++) {
        m_updateDetailItemList.at(i)->setVisible(true);
        if (i == m_updateDetailItemList.count() - 1) {
            m_updateDetailItemList.at(i)->setContentsMargins(5, 15, 20, 30);
        } else {
            m_updateDetailItemList.at(i)->setContentsMargins(5, 15, 20, 10);
        }
        m_lineWidget->setVisible(true);
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

    vector<double> systemVer = getNumListFromStr(Dtk::Core::DSysInfo::minorVersion());
    for (int i = 0; i < detailInfoList.count(); i++) {
        DetailInfoItem *detailInfoItem = new DetailInfoItem(this);
        DetailInfo item = detailInfoList.at(i);
        vector<double> versionVec = getNumListFromStr(item.name);
        if (versionVec.size() < 1 || systemVer.size() < 1 || versionVec.at(0) <= systemVer.at(0)) {
            continue;
        }
        detailInfoItem->setTitle(item.name);
        detailInfoItem->setDate(item.updateTime);
        detailInfoItem->setLinkData(item.link);
        detailInfoItem->setDetailData(item.info);
        detailInfoItem->setVisible(false);
        m_updateDetailItemList.append(detailInfoItem);
        m_settingsGroup->appendItem(detailInfoItem);
    }
}
