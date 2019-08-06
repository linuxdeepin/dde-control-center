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

#include "window/namespace.h"
#include "widgets/contentwidget.h"

#include <types/mirrorinfolist.h>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
    namespace update {
        class UpdateModel;
        class MirrorItem;
    }

    namespace widgets {
        class SettingsGroup;
    }
}

namespace DCC_NAMESPACE{
namespace update{

class MirrorsWidget : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit MirrorsWidget(dcc::update::UpdateModel *model, QWidget* parent = 0);

    void setModel(dcc::update::UpdateModel *model);

protected:
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void requestSetDefaultMirror(const MirrorInfo &mirror);
    void requestTestMirrorSpeed();
    void notifyBackpage();

private Q_SLOTS:
    void setCurItem(dcc::update::MirrorItem* item);
    void onSpeedInfoAvailable(const QMap<QString, int> &info);
    void testButtonClicked();

    void sortMirrorsBySpeed();

private:
    void setDefaultMirror(const MirrorInfo& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

private:
    enum TestProgress {
        NotStarted,
        Running,
        Done
    };

    dcc::update::MirrorItem* m_curItem;
    MirrorInfo m_defaultMirror;
    TestProgress m_testProgress;

    QPushButton *m_testButton;
    dcc::widgets::SettingsGroup *m_mirrorListGroup;
    QVBoxLayout *m_layout;

    Dtk::Widget::DImageButton *m_backBtn;
};

} // namespace update
} // namespace DCC_NAMESPACE

