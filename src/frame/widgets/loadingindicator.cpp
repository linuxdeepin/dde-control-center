// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
