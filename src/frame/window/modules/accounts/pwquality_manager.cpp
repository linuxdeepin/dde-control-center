/*
* Copyright (C) 2017 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangdongdong <zhangdongdong@uniontech.com>
*
* Maintainer: zhangdongdong <zhangdongdong@uniontech.com>
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

#include "pwquality_manager.h"
#include <QStringList>

PwqualityManager::PwqualityManager()
{
    init();
}

void PwqualityManager::init()
{
    m_pwqualitySetting.reset(pwquality_default_settings());
}

PwqualityManager *PwqualityManager::instance()
{
    static PwqualityManager pwquality;
    return &pwquality;
}

QString PwqualityManager::palindromeChecked(const QString &text)
{
    QStringList list;
    for (int pos = 0; pos < text.size() + 1 - m_palindromeLength; pos++) {
        list.append(text.mid(pos, m_palindromeLength));
    }

    for (QString str : list) {
        int code = pwquality_check(m_pwqualitySetting.get(),
                                   str.toStdString().c_str(),
                                   NULL, NULL, NULL);

        if (code == PWQ_ERROR_PALINDROME) {
            return str;
        }
    }

    return QString();
}


QString PwqualityManager::dictChecked(const QString &text)
{
    int code = pwquality_check(m_pwqualitySetting.get(),
                               text.toStdString().c_str(),
                               NULL, NULL, NULL);

    if (code == PWQ_ERROR_CRACKLIB_CHECK) {
        return text;
    }
    return QString();
}
