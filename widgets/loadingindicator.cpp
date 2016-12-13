/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "loadingindicator.h"

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
            QString path = QString(":/widgets/themes/%1/icons/Loading/loading_0%2").arg(theme).arg(arg);
            lists<<path;
        }
        setPictureSequence(lists);
    }
}

} // namespace widgets
} // namespace dcc
