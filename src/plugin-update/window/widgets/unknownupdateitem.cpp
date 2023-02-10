//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "unknownupdateitem.h"

#include <QLayout>
#include <DFontSizeManager>
#include <DLabel>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

UnknownUpdateItem::UnknownUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void UnknownUpdateItem::init()
{
    setIcon(":/icons/deepin/builtin/icons/dcc_unknown_update.svg");
    setClassifyUpdateType(ClassifyUpdateType::UnknownUpdate);
    m_controlWidget->setDetailLabelVisible(false);
    m_controlWidget->setDetailEnable(false);
    m_controlWidget->setShowMoreButtonVisible(false);
    m_controlWidget->setDatetimeVisible(false);
    DLabel *vesrionLabel = m_controlWidget->findChild<DLabel *>("versionLabel");
    vesrionLabel->setEnabled(false);
    auto pal = vesrionLabel->palette();
    QColor base_color = pal.text().color();
    base_color.setAlpha(255 / 10 * 6);
    pal.setColor(QPalette::Text, base_color);
    vesrionLabel->setPalette(pal);
    DFontSizeManager::instance()->bind(vesrionLabel, DFontSizeManager::T8);
    m_controlWidget->layout()->setSpacing(5);
}

void UnknownUpdateItem::setData(UpdateItemInfo *updateItemInfo)
{
    if (updateItemInfo == nullptr) {
        return;
    }

    m_controlWidget->setVersion(updateItemInfo->updateTime().isEmpty() ? "" : tr("Release date: ") + updateItemInfo->updateTime());
    m_controlWidget->setTitle(updateItemInfo->name());

    setProgressVlaue(updateItemInfo->downloadProgress());
    setUpdateSize(updateItemInfo->downloadSize());
}
