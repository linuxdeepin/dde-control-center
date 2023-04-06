//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systemupdateitem.h"
#include "common.h"

#include <DFontSizeManager>
#include <DFrame>
#include <DLabel>
#include <QVBoxLayout>
#include <float.h>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

SystemUpdateItem::SystemUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
    , m_line(new QFrame)
    , m_lineWidget(new QWidget)
{
    setIcon(":/icons/deepin/builtin/icons/dcc_system_update.svg");
    setClassifyUpdateType(SystemUpdate);

    QVBoxLayout *lineLay = new QVBoxLayout;
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

    int lastIndex = -1;

    const QString systemVer = IsCommunitySystem ? Dtk::Core::DSysInfo::deepinVersion() : Dtk::Core::DSysInfo::minorVersion();
    for (int i = 0; i < detailInfoList.count(); i++) {
        const QString currentVersion = detailInfoList.at(i).name;
        if (subVersion(systemVer, currentVersion) > DBL_MIN || subVersion(currentVersion, updateItemInfo->availableVersion()) > DBL_MIN) {
            continue;
        }

        if (IsProfessionalSystem && getLastNumForString(currentVersion) != '0') {
            if (lastIndex < 0 ||  subVersion(currentVersion, detailInfoList.at(lastIndex).name) > DBL_MIN) {
                lastIndex = i;
            }
            continue;
        }

        createDetailInfoItem(detailInfoList, i);
    }

    if (lastIndex > -1 && getLastNumForString(updateItemInfo->availableVersion()) != '0') {
        std::vector<double> firstVersionVec = getNumListFromStr(updateItemInfo->availableVersion());
        std::vector<double> secondVersionVec = getNumListFromStr(detailInfoList.at(lastIndex).name);
        // 当前版本是 1061的话 则不显示1051等类似的小版本
        if (static_cast<int>(firstVersionVec.at(0) / 10) == static_cast<int>(secondVersionVec.at(0) / 10)) {
            createDetailInfoItem(detailInfoList, lastIndex, 0);
        }
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
    std::vector<double> firstVersionVec = getNumListFromStr(firstVersion);
    std::vector<double> secondVersionVec = getNumListFromStr(secondVersion);
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
    DetailInfoItem *detailInfoItem = new DetailInfoItem(this);
    // 根据需求，将最后一个字符替换为0（原因是不想用户看到版本频繁的更新）
    if (IsProfessionalSystem)
          item.name.replace(item.name.length() - 1, 1, '0');
    detailInfoItem->setTitle(item.name);
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
