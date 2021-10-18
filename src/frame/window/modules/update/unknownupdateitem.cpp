#include "unknownupdateitem.h"

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
    m_controlWidget->setVersionVisible(false);
}
