#include "unknownupdateitem.h"

#include <DFontSizeManager>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;


UnknownUpdateItem::UnknownUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void UnknownUpdateItem::init()
{
    setIcon(":/update/updatev20/dcc_unknown_update.svg");
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

    m_controlWidget->setVersion(tr("Release date: ") + updateItemInfo->updateTime());
    m_controlWidget->setTitle(updateItemInfo->name());

    setProgressVlaue(updateItemInfo->downloadProgress());
    setUpdateSize(updateItemInfo->downloadSize());
}
