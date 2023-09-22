//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "touchscreenmodule.h"
#include "dtiplabel.h"
#include "itemmodule.h"
#include "settingsgroupmodule.h"
#include "touchscreenmodel.h"
#include "buttontuple.h"

#include <DPushButton>
#include <DIconTheme>

#include <QComboBox>
#include <QPushButton>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

TouchScreenModule::TouchScreenModule(QObject *parent)
    : PageModule("touchscreen", tr("Touch Screen"), tr("Touch Screen"), DIconTheme::findQIcon("dcc_nav_touchscreen"), parent)
{
    m_model  = new TouchScreenModel(this);
    init();
}

TouchScreenModule::~TouchScreenModule()
{

}

void TouchScreenModule::save()
{
    bool changed = false;
    for (auto item : qAsConst(m_itemMap)) {
        QString UUID = item->property("touchScreeUUID").toString();
        QString monitor = item->currentText();
        TouchscreenMap map = m_model->touchMap();
        if (map.value(UUID) == monitor)
            continue;

        m_model->assoiateTouch(monitor, UUID);
        changed = true;
    }

    if (changed)
        m_model->assoiateTouchNotify();
}

void TouchScreenModule::init()
{
    m_monitors = m_model->monitors();
    m_touchScreens = m_model->touchScreenList();
    PageModule *page = new PageModule("touchScreenPage", tr("Touch Screen"));
    page->setContentsMargins(0, 0, 0, 0);
    appendChild(page);
    page->appendChild(new ItemModule("touchScreenTipLabel", tr("Select your touch screen when connected or set it here."),
        [] (ModuleObject *module) -> QWidget*{
            DTipLabel *tipLabel = new DTipLabel(module->displayName());
            tipLabel->setWordWrap(true);
            tipLabel->setContentsMargins(10, 0, 0, 0);
            tipLabel->setAlignment(Qt::AlignLeft);
            return tipLabel;
        }, false));
    m_settingGroup = new SettingsGroupModule("touchScreenSettingGroup", QString());
    page->appendChild(m_settingGroup);

    connect(m_model, &TouchScreenModel::touchMapChanged, this, &TouchScreenModule::resetItems);
    connect(m_model, &TouchScreenModel::touchScreenListChanged, this, &TouchScreenModule::resetItems);
    connect(m_model, &TouchScreenModel::monitorsChanged, this, &TouchScreenModule::resetItems);
    resetItems();

    ItemModule *cancel = new ItemModule("Cancel","Cancel",[this](ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            ButtonTuple *btnTuple = new ButtonTuple(ButtonTuple::Save);
            btnTuple->setBackgroundRole(QPalette::Base);
            QPushButton *confirm = btnTuple->rightButton();
            QPushButton *cancel = btnTuple->leftButton();
            confirm->setText(tr("Confirm"));
            cancel->setText(tr("Cancel"));
            confirm->setFixedSize(200, 36);
            cancel->setFixedSize(200, 36);
            confirm->setEnabled(false);
            connect(confirm, &QPushButton::clicked, this, [this, confirm](){
                this->save();
                confirm->setEnabled(false);
            });
            connect(cancel, &QPushButton::clicked, this, &TouchScreenModule::resetItems);
            connect(this, &TouchScreenModule::onChanged, confirm, [confirm](){
                confirm->setEnabled(true);
            });
            return btnTuple;
        }, false);
    cancel->setExtra();
    appendChild(cancel);
}

void TouchScreenModule::resetItems()
{
    while (0 < m_settingGroup->getChildrenSize()) {
        m_settingGroup->removeChild(0);
    }


    m_itemMap.clear();
    if (m_model->monitors().empty() || m_model->touchScreenList().empty()) {
        this->setHidden(true);
    }
    else
        this->setHidden(false);

    auto touchScreenAndMonitor = m_model->touchMap();

    QStringList monitors = m_model->monitors();

    for (const auto &touchScreen : m_model->touchScreenList()) {
        ItemModule *item = new ItemModule("touchScreen", touchScreen.name,
                                          [monitors, touchScreen, touchScreenAndMonitor, this] (ModuleObject *module) -> QWidget*{
                                              Q_UNUSED(module)
                                              QComboBox *match = new QComboBox();
                                              match->addItems(monitors);
                                              match->setProperty("touchScreeUUID",touchScreen.UUID);
                                              if (touchScreenAndMonitor.contains(touchScreen.UUID))
                                                  match->setCurrentText(touchScreenAndMonitor.value(touchScreen.UUID));
                                              m_itemMap.insert(touchScreen.name,match);
                                              connect(match, &QComboBox::currentTextChanged, this, &TouchScreenModule::onChanged);
                                              return match;
                                          });

        m_settingGroup->appendChild(item);
    }
}

QString dccV23::TouchScreenPlugin::name() const
{
    return QStringLiteral("touchscreen");
}

ModuleObject *TouchScreenPlugin::module()
{
    return new TouchScreenModule();;
}

QString TouchScreenPlugin::location() const
{
    return "display";
}
