/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "widgets/loadingindicator.h"

namespace dcc {
namespace widgets {

LoadingIndicator::LoadingIndicator(QWidget *parent) :
    DPictureSequenceView(parent)
{
//    setTheme("dark");
}

void LoadingIndicator::setTheme(const QString &theme)
{
    if (theme != m_theme) {
        m_theme = theme;

        QStringList lists;
        for(int i = 0; i<89; i++)
        {
            QString arg = QString().setNum(i);
            if(arg.length() == 1)
                arg = "0"+arg;
            QString path = QString(":/widgets/themes/%1/icons/Loading/loading_0%2.png").arg(theme).arg(arg);
            lists<<path;
        }
        setPictureSequence(lists);
    }
}

} // namespace widgets
} // namespace dcc
