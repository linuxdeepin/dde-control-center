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

#ifndef INDICATORWIDGET_H
#define INDICATORWIDGET_H

#include <QWidget>

#include <dpageindicator.h>
#include <dimagebutton.h>

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IndicatorWidget(QWidget *parent = 0);

    void setPageCount(const int count) { m_pluginsIndicator->setPageCount(count); }
    void setCurrentPage(const int page) { m_pluginsIndicator->setCurrentPage(page); }

Q_SIGNALS:
    void requestPrevious();
    void requestNext();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    Dtk::Widget::DPageIndicator *m_pluginsIndicator;
    Dtk::Widget::DImageButton *m_prevPluginBtn;
    Dtk::Widget::DImageButton *m_nextPluginBtn;

    QTimer *m_ignoreWheelRepeat;
};

#endif // INDICATORWIDGET_H
