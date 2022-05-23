/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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

#ifndef PasswordWidget_H
#define PasswordWidget_H

#include <QLabel>

#include <DPasswordEdit>
#include <DGuiApplicationHelper>

#include "deepin_pw_check.h"
#include "utils.h"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

class PasswordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordWidget(int margin, const QString &userName, QWidget *parent = nullptr);
    ~PasswordWidget() {}

    bool checkPassword();
    inline const QString getPassword() { return m_newPasswordEdit->text(); }
    bool isPasswordEmpty();

private:
    void initWidget(int margin);
    void initData();
    void hideAlert(DLineEdit *edit);

Q_SIGNALS:
    void requestSetPasswordHint(const QString &);

private:
    QString m_userName;
    DPasswordEdit *m_newPasswordEdit;
    DPasswordEdit *m_repeatPasswordEdit;
    DLineEdit *m_passwordTipsEdit;
    QLabel *m_newPasswdLevelText;
};

#endif // REMINDERDDIALOG_H
