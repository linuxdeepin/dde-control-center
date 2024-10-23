// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationdesktopmodule.h"

#include "dccslider.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "titledslideritem.h"
#include "widgets/horizontalmodule.h"
#include "widgets/itemmodule.h"
#include "widgets/settingsgroupmodule.h"
#include "widgets/widgetmodule.h"
#include "widgets/switchwidget.h"

#include <DSwitchButton>
#include <DSysInfo>
#include <DIconTheme>
#include <DTipLabel>

#include <QComboBox>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

PersonalizationDesktopModule::PersonalizationDesktopModule(PersonalizationModel *model,
                                                           PersonalizationWorker *work,
                                                           QObject *parent)
    : PageModule("desktop", tr("Desktop"), parent)
    , m_model(model)
    , m_work(work)
{
    if (DSysInfo::UosServer != DSysInfo::uosType()) { // 以下服务器版没有
        appendChild(new ItemModule("window", tr("Window")));
        SettingsGroupModule *group = new SettingsGroupModule("windowGroup", tr("Window"));
        appendChild(group);
        if (!qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland"))
            group->appendChild(new ItemModule("windowEffect",
                                              tr("Window Effect"),
                                              this,
                                              &PersonalizationDesktopModule::initWindowEffect));

        ItemModule *itemMinimizeEffect =
                new ItemModule("minimizeEffect",
                               tr("Window Minimize Effect"),
                               this,
                               &PersonalizationDesktopModule::initMiniEffect);
        group->appendChild(itemMinimizeEffect);

        auto compactModule = new ItemModule(
            "compactDisplay",
            tr("Compact Display"),
            this,
            &PersonalizationDesktopModule::initCompactMode);

        compactModule->setBackground(true);
        appendChild(compactModule);

        auto compactDisplayTipModule = new WidgetModule<DTipLabel>(
        "compactDisplayTip",
        tr(""),
        [](DTipLabel *label) {
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignLeft);
            label->setContentsMargins(10, 0, 10, 0);
            label->setText(tr("If enabled, more content is displayed in the window."));
        });
        appendChild(compactDisplayTipModule);
    
        HorizontalModule *hor = new HorizontalModule(QString(), QString());
        appendChild(hor);
        hor->appendChild(new ItemModule("transparencyEffect",
                                        tr("Transparency"),
                                        this,
                                        &PersonalizationDesktopModule::initTransparentEffect,
                                        false));
        hor->appendChild(new ItemModule("roundedEffect",
                                        tr("Rounded Corner"),
                                        this,
                                        &PersonalizationDesktopModule::initRoundEffect,
                                        false));
        itemMinimizeEffect->setVisible(m_model->is3DWm());
        hor->setVisible(m_model->is3DWm());
        connect(m_model,
                &PersonalizationModel::wmChanged,
                itemMinimizeEffect,
                &ItemModule::setVisible);
        connect(m_model, &PersonalizationModel::wmChanged, hor, &ItemModule::setVisible);
    }
}

QWidget *PersonalizationDesktopModule::initWindowEffect(ModuleObject *module)
{
    Q_UNUSED(module)
    DSwitchButton *wmSwitch = new DSwitchButton();
    wmSwitch->setChecked(m_model->is3DWm());
    connect(m_model, &PersonalizationModel::wmChanged, wmSwitch, &DSwitchButton::setChecked);
    connect(wmSwitch, &DSwitchButton::checkedChanged, this, [this, wmSwitch](bool checked) {
        wmSwitch->setChecked(m_model->is3DWm());
        m_work->windowSwitchWM(checked);
        m_work->setMiniEffect(m_model->miniEffect());
    });
    return wmSwitch;
}

QWidget *PersonalizationDesktopModule::initTransparentEffect(ModuleObject *module)
{
    Q_UNUSED(module)
    TitledSliderItem *transparentSlider = new TitledSliderItem();
    transparentSlider->setTitle(tr("Opacity"));
    transparentSlider->addBackground();
    transparentSlider->slider()->setOrientation(Qt::Horizontal);
    transparentSlider->setObjectName("Transparency");

    // 设计效果图变更：增加左右图标
    transparentSlider->setLeftIcon(DIconTheme::findQIcon("transparency_low"));
    transparentSlider->setRightIcon(DIconTheme::findQIcon("transparency_high"));
    transparentSlider->setIconSize(QSize(24, 24));
    DCCSlider *slider = transparentSlider->slider();
    slider->setAccessibleName("transparency");
    slider->setRange(0, 100);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    auto onOpacityChanged = [transparentSlider](std::pair<int, double> value) {
        transparentSlider->slider()->blockSignals(true);
        transparentSlider->slider()->setValue(value.first);
        transparentSlider->slider()->blockSignals(false);
        transparentSlider->setValueLiteral(QString("%1").arg(value.first));
    };
    onOpacityChanged(m_model->opacity());
    connect(m_model, &PersonalizationModel::onOpacityChanged, transparentSlider, onOpacityChanged);
    connect(transparentSlider->slider(),
            &DCCSlider::valueChanged,
            m_work,
            &PersonalizationWorker::setOpacity);
    connect(transparentSlider->slider(),
            &DCCSlider::sliderMoved,
            m_work,
            &PersonalizationWorker::setOpacity);
    return transparentSlider;
}

QWidget *PersonalizationDesktopModule::initMiniEffect(ModuleObject *module)
{
    Q_UNUSED(module)
    QComboBox *cmbMiniEffect = new QComboBox();
    cmbMiniEffect->addItem(tr("Scale"));
    cmbMiniEffect->addItem(tr("Magic Lamp"));
    cmbMiniEffect->setCurrentIndex(m_model->miniEffect());
    connect(cmbMiniEffect,
            qOverload<int>(&QComboBox::currentIndexChanged),
            m_work,
            &PersonalizationWorker::setMiniEffect);
    connect(m_model,
            &PersonalizationModel::onMiniEffectChanged,
            cmbMiniEffect,
            &QComboBox::setCurrentIndex);
    return cmbMiniEffect;
}

QWidget *PersonalizationDesktopModule::initRoundEffect(ModuleObject *module)
{
    Q_UNUSED(module)
    TitledSliderItem *winRoundSlider = new TitledSliderItem();
    winRoundSlider->setTitle(tr("Rounded Corner"));
    winRoundSlider->addBackground();
    winRoundSlider->slider()->setOrientation(Qt::Horizontal);
    winRoundSlider->setObjectName("winRoundSlider");
    winRoundSlider->setIconSize(QSize(32, 32));
    winRoundSlider->setLeftIcon(DIconTheme::findQIcon("round_low"));
    winRoundSlider->setRightIcon(DIconTheme::findQIcon("round_high"));

    DCCSlider *sliderRound = winRoundSlider->slider();
    sliderRound->setType(DCCSlider::Vernier);
    sliderRound->setTickPosition(QSlider::TicksBelow);
    sliderRound->setRange(0, 2);
    sliderRound->setTickInterval(1);
    sliderRound->setPageStep(1);
    auto onWindowRadiusChanged = [winRoundSlider](int radius) {
        if (radius <= 0) {
            winRoundSlider->slider()->setValue(0);
            winRoundSlider->setValueLiteral(tr("Small"));
        } else if (radius <= 8) {
            winRoundSlider->slider()->setValue(1);
            winRoundSlider->setValueLiteral(tr("Middle"));
        } else {
            winRoundSlider->slider()->setValue(2);
            winRoundSlider->setValueLiteral(tr("Large"));
        }
    };
    onWindowRadiusChanged(m_model->windowRadius());
    connect(m_model,
            &PersonalizationModel::onWindowRadiusChanged,
            sliderRound,
            onWindowRadiusChanged);
    connect(winRoundSlider->slider(), &DCCSlider::valueChanged, this, [=](int value) {
        int val = value;
        switch (value) {
        case 0:
            val = 0;
            break;
        case 1:
            val = 8;
            break;
        case 2:
            val = 18;
            break;
        }
        m_work->setWindowRadius(val);
    });
    return winRoundSlider;
}

QWidget *PersonalizationDesktopModule::initCompactMode(ModuleObject *module)
{
    Q_UNUSED(module)
    DSwitchButton *switchBtn = new DSwitchButton();
    switchBtn->setChecked(m_model->getCompactDisplay());
    connect(m_model, &PersonalizationModel::compactDisplayChanged, switchBtn, &DSwitchButton::setChecked);
    connect(switchBtn, &DSwitchButton::checkedChanged, this, [this](bool checked) {
        m_work->setCompactDisplay(checked);
    });
    return switchBtn;
}
