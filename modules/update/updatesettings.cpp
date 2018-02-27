/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include <QVBoxLayout>

#include "settingsgroup.h"
#include "updatemodel.h"
#include "translucentframe.h"
#include "labels/smalllabel.h"

namespace dcc{
namespace update{

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_model(nullptr)
{
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    QSettings setting("/etc/deepin-version", QSettings::IniFormat);
    setting.beginGroup("Release");
    m_isProfessional = setting.value("Type").toString() == "Professional";
#else
    m_isProfessional = true;
#endif

    setTitle(tr("Update Settings"));

    TranslucentFrame* widget = new TranslucentFrame;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);

    SettingsGroup* ug = new SettingsGroup;

    m_autoCleanCache = new SwitchWidget;
    m_autoCleanCache->setTitle(tr("Auto clear package cache"));

    m_autoDownloadSwitch = new SwitchWidget;
    m_autoDownloadSwitch->setTitle(tr("Auto-download Updates"));

    TipsLabel* label = new TipsLabel(tr("Updates will be auto-downloaded in wireless or wired network"));
    label->setWordWrap(true);
    label->setContentsMargins(20, 0, 20, 0);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    SettingsGroup* sourceCheckGrp = new SettingsGroup;
    m_sourceCheck = new SwitchWidget;
    m_sourceCheck->setTitle(tr("System source detection"));
    sourceCheckGrp->appendItem(m_sourceCheck);
    layout->addWidget(sourceCheckGrp);
    layout->addSpacing(8);
    TipsLabel *sourceCheckLbl = new TipsLabel(tr("Prompt the notification if system source has been modified"));
    sourceCheckLbl->setWordWrap(true);
    sourceCheckLbl->setContentsMargins(20, 0, 20, 0);
    layout->addWidget(sourceCheckLbl);
    layout->addSpacing(8);
#endif

    ug->appendItem(m_autoCleanCache);
    ug->appendItem(m_autoDownloadSwitch);

    layout->addWidget(ug);
    layout->addSpacing(8);
    layout->addWidget(label);
    layout->addSpacing(15);

    if (!m_isProfessional) {
        m_updateMirrors = new NextPageWidget;
        m_updateMirrors->setTitle(tr("Switch Mirror"));
        SettingsGroup* mg = new SettingsGroup;
        mg->appendItem(m_updateMirrors);
        layout->addWidget(mg);
    }

    layout->addStretch();

    widget->setLayout(layout);

    setContent(widget);

    if (!m_isProfessional) {
        connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
    }

    connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCleanCache);
    connect(m_autoDownloadSwitch, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoUpdate);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(m_sourceCheck, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetSourceCheck);
#endif

    setModel(model);
}

void UpdateSettings::setModel(UpdateModel *model)
{
    if (model) {
        m_model = model;

        auto setAutoDownload = [this] (const bool &autoDownload) {
            m_autoDownloadSwitch->setChecked(autoDownload);
        };

        if (!m_isProfessional) {
            auto setDefaultMirror = [this] (const MirrorInfo &mirror) {
                m_updateMirrors->setValue(mirror.m_name);
            };
            setDefaultMirror(model->defaultMirror());
            connect(model, &UpdateModel::defaultMirrorChanged, this, setDefaultMirror);
        }

        setAutoDownload(model->autoDownloadUpdates());

        connect(model, &UpdateModel::autoDownloadUpdatesChanged, this, setAutoDownload);
        connect(model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);

        m_autoCleanCache->setChecked(m_model->autoCleanCache());

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
        connect(model, &UpdateModel::sourceCheckChanged, m_sourceCheck, &SwitchWidget::setChecked);
        m_sourceCheck->setChecked(model->sourceCheck());
#endif
    }
}

}
}
