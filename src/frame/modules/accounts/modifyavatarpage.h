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

#ifndef MODIFYAVATARPAGE_H
#define MODIFYAVATARPAGE_H

#include "widgets/contentwidget.h"
#include "user.h"

#include <QGridLayout>

namespace dcc {
namespace accounts {

class ModifyAvatarPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyAvatarPage(User *user, QWidget *parent = 0);

Q_SIGNALS:
    void requestAddNewAvatar(User *user) const;
    void requestSetAvatar(User *user, const QString &filePath) const;
    void requestDeleteAvatar(User *user, const QString &iconPath) const;

private:
    void appendAvatar(const QString &avatar, const int index, const bool selected, const bool deletable = false);

private Q_SLOTS:
    void updateAvatarList();
    void updateTitle();

private:
    User *m_userModel;
    QGridLayout *m_avatarsLayout;
};

} // namespace accounts
} // namespace dcc

#endif // MODIFYAVATARPAGE_H
