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
#pragma once

#include "window/interface/moduleinterface.h"
#include "widgets/contentwidget.h"

namespace dcc {
    namespace widgets {
        class SettingsGroup;
        class SwitchWidget;
        class NextPageWidget;
        class TipsLabel;
    }

    namespace update {
        class UpdateModel;
    }
}

namespace DCC_NAMESPACE {
namespace update{

class UpdateSettings : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(dcc::update::UpdateModel* model, QWidget *parent = 0);

    void setModel(dcc::update::UpdateModel *model);

Q_SIGNALS:
    void requestShowMirrorsView();
    void requestSetAutoUpdate(const bool &autoUpdate);
    void requestSetAutoCleanCache(const bool autoClean);
    void requestSetAutoCheckUpdates(const bool autocheckUpdate);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void requestSetSourceCheck(const bool check);
#endif
    void requestEnableSmartMirror(bool enable);

private:
    dcc::update::UpdateModel* m_model;
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    dcc::widgets::SwitchWidget *m_sourceCheck;
#endif
    dcc::widgets::SwitchWidget *m_autoCleanCache;
    dcc::widgets::SwitchWidget *m_autoCheckUpdate;
    dcc::widgets::SwitchWidget* m_autoDownloadSwitch;
    dcc::widgets::SwitchWidget *m_smartMirrorBtn;
    dcc::widgets::NextPageWidget* m_updateMirrors;
    dcc::widgets::TipsLabel *m_updateLbl;
    dcc::widgets::SettingsGroup *m_mirrorGrp;
    bool m_isProfessional;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
