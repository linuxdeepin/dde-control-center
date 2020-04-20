/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "updatesettings.h"
#include "widgets/settingsgroup.h"
#include "modules/update/updatemodel.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/smalllabel.h"
#include "widgets/switchwidget.h"
#include "widgets/nextpagewidget.h"
#include "dsysinfo.h"
#include "window/utils.h"

#include <DTipLabel>

#include <QVBoxLayout>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(nullptr)
{
    setTitle(tr("Update Settings"));

    TranslucentFrame *widget = new TranslucentFrame;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(71);

    SettingsGroup *ug = new SettingsGroup;

    m_autoCleanCache = new SwitchWidget;
    //~ contents_path /update/Update Settings
    m_autoCleanCache->setTitle(tr("Auto Clear Package Cache"));

    m_autoCheckUpdate = new SwitchWidget;
    //~ contents_path /update/Update Settings
    m_autoCheckUpdate->setTitle(tr("Updates Notification"));

    m_autoDownloadSwitch = new SwitchWidget;
    m_autoDownloadSwitch->setTitle(tr("Auto Download Updates"));

    m_updateLbl = new DTipLabel(tr("Switch it on to automatically download the updates in wireless or wired network"));
    m_updateLbl->setWordWrap(true);
    m_updateLbl->setAlignment(Qt::AlignLeft);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal") {
        SettingsGroup *sourceCheckGrp = new SettingsGroup;
        m_sourceCheck = new SwitchWidget;
        //~ contents_path /update/Update Settings
        m_sourceCheck->setTitle(tr("System Repository Detection"));
        sourceCheckGrp->appendItem(m_sourceCheck);
        layout->addWidget(sourceCheckGrp);
        layout->addSpacing(8);
        DTipLabel *sourceCheckLbl = new DTipLabel(tr("Show a notification if system update repository has been modified"));
        sourceCheckLbl->setWordWrap(true);
        sourceCheckLbl->setAlignment(Qt::AlignLeft);
        QHBoxLayout *sourceCheckLblLayout = new QHBoxLayout;
        sourceCheckLblLayout->addSpacing(TipLeftInterver);
        sourceCheckLblLayout->addWidget(sourceCheckLbl);
        layout->addLayout(sourceCheckLblLayout);
        layout->addSpacing(8);
    }
#endif

    ug->setSpacing(List_Interval);
    ug->appendItem(m_autoCleanCache);
    ug->appendItem(m_autoCheckUpdate);
    ug->appendItem(m_autoDownloadSwitch);

    layout->addWidget(ug);
    layout->addSpacing(8);
    QHBoxLayout *updateLblLayout = new QHBoxLayout;
    updateLblLayout->addSpacing(TipLeftInterver);
    updateLblLayout->addWidget(m_updateLbl);
    layout->addLayout(updateLblLayout);
    layout->addSpacing(15);

    if (SystemTypeName != "Professional" && SystemTypeName != "Personal") {
        m_smartMirrorBtn = new SwitchWidget;
        //~ contents_path /update/Update Settings
        m_smartMirrorBtn->setTitle(tr("Smart Mirror Switch"));

        SettingsGroup *smartMirrorGrp = new SettingsGroup;
        smartMirrorGrp->appendItem(m_smartMirrorBtn);

        DTipLabel *smartTips = new DTipLabel("");
        //~ contents_path /update/Update Settings
        smartTips->setText(tr("Switch it on to connect to the quickest mirror site automatically"));
        smartTips->setWordWrap(true);
        smartTips->setAlignment(Qt::AlignLeft);
        layout->addWidget(smartMirrorGrp);
        layout->addSpacing(8);
        QHBoxLayout *smartTipsLayout = new QHBoxLayout;
        smartTipsLayout->addSpacing(TipLeftInterver);
        smartTipsLayout->addWidget(smartTips);
        layout->addLayout(smartTipsLayout);
        layout->addSpacing(15);

        m_updateMirrors = new NextPageWidget(nullptr, false);
        m_updateMirrors->setRightTxtWordWrap(true);
        //~ contents_path /update/Update Settings/Mirror List
        m_updateMirrors->setTitle(tr("Mirror List"));
        m_mirrorGrp = new SettingsGroup;
        m_mirrorGrp->appendItem(m_updateMirrors);
        layout->addWidget(m_mirrorGrp);

        connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
        connect(m_smartMirrorBtn, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestEnableSmartMirror);
    }

    layout->addStretch();

    widget->setLayout(layout);

    setContent(widget);

    connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCleanCache);
    connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckUpdates);
    connect(m_autoDownloadSwitch, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoUpdate);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal") {
        connect(m_sourceCheck, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetSourceCheck);
    }
#endif

    setModel(model);
}

void UpdateSettings::setModel(UpdateModel *model)
{
    m_model = model;

    auto setAutoDownload = [this](const bool & autoDownload) {
        m_autoDownloadSwitch->setChecked(autoDownload);
    };

    if (SystemTypeName != "Professional" && SystemTypeName != "Personal") {
        auto setDefaultMirror = [this](const MirrorInfo & mirror) {
            m_updateMirrors->setValue(mirror.m_name);
        };

        if (!model->mirrorInfos().isEmpty()) {
            setDefaultMirror(model->defaultMirror());
        }

        connect(model, &UpdateModel::defaultMirrorChanged, this, setDefaultMirror);
        connect(model, &UpdateModel::smartMirrorSwitchChanged, m_smartMirrorBtn, &SwitchWidget::setChecked);
        m_smartMirrorBtn->setChecked(m_model->smartMirrorSwitch());

        auto setMirrorListVisible = [ = ](bool visible) {
            m_mirrorGrp->setVisible(!visible);
        };

        connect(model, &UpdateModel::smartMirrorSwitchChanged, this, setMirrorListVisible);
        setMirrorListVisible(model->smartMirrorSwitch());
    }

    setAutoDownload(model->autoDownloadUpdates());

    connect(model, &UpdateModel::autoDownloadUpdatesChanged, this, setAutoDownload);
    connect(model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoDownloadSwitch, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_updateLbl, &DTipLabel::setVisible);

    m_autoDownloadSwitch->setVisible(model->autoCheckUpdates());
    m_autoCheckUpdate->setChecked(model->autoCheckUpdates());
    m_autoCleanCache->setChecked(m_model->autoCleanCache());
    m_updateLbl->setVisible(model->autoCheckUpdates());

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (SystemTypeName != "Server" && SystemTypeName != "Professional" && SystemTypeName != "Personal") {
        connect(model, &UpdateModel::sourceCheckChanged, m_sourceCheck, &SwitchWidget::setChecked);
        m_sourceCheck->setChecked(model->sourceCheck());
    }
#endif
}
