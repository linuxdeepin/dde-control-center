// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QObject>

#include <unistd.h>
#include <random>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

#define PASSWORD_LEVEL_ICON_NUM 3
#define PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_light_mode.svg"
#define PASSWORD_LEVEL_ICON_DEEP_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_deep_mode.svg"
#define PASSWORD_LEVEL_ICON_LOW_PATH ":/icons/dcc_deepin_password_strength_low.svg"
#define PASSWORD_LEVEL_ICON_MIDDLE_PATH ":/icons/dcc_deepin_password_strength_middle.svg"
#define PASSWORD_LEVEL_ICON_HIGH_PATH ":/icons/dcc_deepin_password_strength_high.svg"

#define BIND_ICON_LIGHT ":/icons/unbind_light.svg"
#define BIND_ICON_DARK ":/icons/unbind_dark.svg"

/**
 * @brief crypt函数是否支持SM3算法
 * crypt使用固定的key和salt进行加密，把加密结果与已知的正确结果进行比对，相等则支持，反之不支持
 * @return true 支持
 * @return false 不支持
 */
inline bool supportSM3()
{
    char password[] = "Hello world!";
    char salt[] = "$8$saltstring";
    const QString cryptResult = "$8$saltstring$6RCuSuWbADZmLALkvsvtcYYzhrw3xxpuDcqwdPIWxTD";
    return crypt(password, salt) == cryptResult;
}

inline QString cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */
    const QString seedChars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";
    if (supportSM3()) {
        salt[1] = '8';
    }

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedChars.size() - 1); //seedChars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedChars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}

inline bool isContentEmpty(DLineEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
        edit->setAlert(true);
        edit->showAlertMessage(QObject::tr("It cannot be empty"), edit, 2000);
    } else {
        edit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
        edit->setAlert(false);
    }

    return edit->text().isEmpty();
}
#endif // V20_DISPLAY_UTILS_H
