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

#ifndef ADDIALOG_H
#define ADDIALOG_H

#include "../usermodel.h"

#include <ddialog.h>
#include <QWidget>
#include <QLineEdit>
#include <dpasswordedit.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {

class ADDialog : public DDialog
{
    Q_OBJECT
public:
    explicit ADDialog(QWidget *parent = nullptr);

    void setUserModel(UserModel *model);

Q_SIGNALS:
    void requestInfos(const QString &server, const QString& account, const QString &password);

private:
    void initUI();
    void setVisible(bool visible) override;

    enum ButtonType {
        Cancle = 0,
        Submit = 1
    };

private:
    DLineEdit *m_server;
    DLineEdit *m_username;
    DPasswordEdit *m_password;
    UserModel *m_model;
    ButtonType m_clickType;
};
}
}

#endif // ADDIALOG_H
