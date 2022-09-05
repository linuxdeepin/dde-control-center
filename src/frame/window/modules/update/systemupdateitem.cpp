// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <DFontSizeManager>
#include <float.h>

#include "systemupdateitem.h"
#include "window/utils.h"

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
    if (updateItemInfo->availableVersion().isEmpty() && updateItemInfo->updateTime().isEmpty()) {
        m_controlWidget->setDetailLabelVisible(false);
        m_controlWidget->setDetailEnable(false);
        m_controlWidget->setShowMoreButtonVisible(false);
        m_controlWidget->setDatetimeVisible(false);
        DLabel *vesrionLabel = m_controlWidget->findChild<DLabel *>("versionLabel");
        vesrionLabel->setVisible(true);
        vesrionLabel->setText(updateItemInfo->explain());
        vesrionLabel->setContentsMargins(0, 4, 0, 0);
        DFontSizeManager::instance()->bind(vesrionLabel, DFontSizeManager::T8);
        vesrionLabel->setForegroundRole(DPalette::TextTips);
    }

    QList<DetailInfo> detailInfoList = updateItemInfo->detailInfos();

    if (!m_updateDetailItemList.isEmpty()) {
        for (DetailInfoItem *item : m_updateDetailItemList) {
            m_settingsGroup->removeItem(item);
        }
        qDeleteAll(m_updateDetailItemList);
        m_updateDetailItemList.clear();
    }

    const QString systemVer = dccV20::IsCommunitySystem ? Dtk::Core::DSysInfo::deepinVersion() : Dtk::Core::DSysInfo::minorVersion();
    for (int i = 0; i < detailInfoList.count(); i++) {
        const QString currentVersion = detailInfoList.at(i).name;
        if (subVersion(systemVer, currentVersion) > DBL_MIN || subVersion(currentVersion, updateItemInfo->availableVersion()) > DBL_MIN) {
            continue;
        }

        createDetailInfoItem(detailInfoList, i);
    }

    m_controlWidget->setShowMoreButtonVisible(m_updateDetailItemList.count());
}

char SystemUpdateItem::getLastNumForString(const QString &value)
{
    QChar lastNum = QChar::Null;
    for (QChar item : value) {
        if (item.toLatin1() >= '0' && item.toLatin1() <= '9') {
            lastNum = item;
        }
    }

    return lastNum.toLatin1();
}

double SystemUpdateItem::subVersion(const QString &firstVersion, const QString &secondVersion)
{
    vector<double> firstVersionVec = getNumListFromStr(firstVersion);
    vector<double> secondVersionVec = getNumListFromStr(secondVersion);
    if (firstVersionVec.empty() || secondVersionVec.empty()) {
        return -1;
    }

    return firstVersionVec.at(0) - secondVersionVec.at(0);
}

void SystemUpdateItem::createDetailInfoItem(const QList<DetailInfo> &detailInfoList, int index, int groupIndex)
{
    if (index >= detailInfoList.count() || index < 0) {
        return;
    }
    DetailInfo item = detailInfoList.at(index);
    const QString &systemVersionType = DCC_NAMESPACE::IsServerSystem ? tr("Server") : tr("Desktop");
    DetailInfoItem *detailInfoItem = new DetailInfoItem(this);
    // 根据需求，将最后一个字符替换为0（原因是不想用户看到版本频繁的更新）
    if (dccV20::IsProfessionalSystem)
        item.name.replace(item.name.length() - 1, 1, '0');
    detailInfoItem->setTitle(systemVersionType + item.name);
    detailInfoItem->setDate(item.updateTime);
    detailInfoItem->setLinkData(item.link);
    detailInfoItem->setDetailData(item.info);
    detailInfoItem->setVisible(false);
    if (groupIndex < 0) {
        m_updateDetailItemList.append(detailInfoItem);
        m_settingsGroup->appendItem(detailInfoItem);
    } else {
        m_updateDetailItemList.insert(groupIndex, detailInfoItem);
        m_settingsGroup->insertItem(groupIndex + 2, detailInfoItem);
    }
}
