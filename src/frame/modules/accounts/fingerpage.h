/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#ifndef FINGERPAGE_H
#define FINGERPAGE_H

#include "widgets/contentwidget.h"
#include "user.h"
#include "fingermodel.h"

#include <com_deepin_daemon_fprintd_device.h>

using com::deepin::daemon::fprintd::Device;

namespace dcc {

namespace widgets {
class SettingsGroup;
}

namespace accounts {

class FingerPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit FingerPage(User *user,QWidget *parent = nullptr);

    void setFingerModel(FingerModel *model);

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestCleanThumbs(User *user);

private Q_SLOTS:
    void onThumbsListChanged(const QList<FingerModel::UserThumbs> &thumbs);
    void addThumb();
    void cleanThumbs();

private:
    widgets::SettingsGroup *m_listGrp;
    User* m_user;
    FingerModel *m_model;
    QPushButton *m_addBtn;
    QPushButton *m_cleanBtn;
    QString m_notUseThumb;
};

}
}


#endif // FINGERPAGE_H
