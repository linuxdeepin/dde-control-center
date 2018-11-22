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

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include "widgets/settingsitem.h"

#include <types/appupdateinfolist.h>

#include <QLabel>

class QVBoxLayout;
class QPushButton;

namespace dcc {
namespace widgets {
class SmallLabel;
}
}

namespace dcc{
namespace update{

class UpdateItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit UpdateItem(QFrame* parent = 0);

    void setAppInfo(const AppUpdateInfo& info);

    static bool isAnchor(const QString &input);
    static QPair<QString, QString> parseAnchor(const QString &input);

    QSize sizeHint() const override;

private:
    AppUpdateInfo m_info;

    dcc::widgets::SmallLabel *m_appIcon;
    dcc::widgets::SmallLabel *m_appName;
    dcc::widgets::SmallLabel *m_appVersion;
    dcc::widgets::SmallLabel *m_appChangelog;

    QPushButton *m_details;
    QVBoxLayout *m_iconLayout;

    QString elidedChangelog() const;
    void expandChangelog();
};

}
}
#endif // UPDATEITEM_H
