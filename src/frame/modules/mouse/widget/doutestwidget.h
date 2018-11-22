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

#ifndef DOUTESTWIDGET_H
#define DOUTESTWIDGET_H

#include "widgets/settingsitem.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>
#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

class QMouseEvent;
namespace dcc
{
namespace widgets
{
class SettingsItem;
}
namespace mouse
{
class DouTestWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DouTestWidget(QWidget *parent = 0);

    enum State {
        BOW,RAISE
    };

    struct DoubleTestPic {
        QStringList double_1;
        QStringList double_2;
        QStringList click_1;
        QStringList click_2;
    };

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QVBoxLayout           *m_mainlayout;
    DPictureSequenceView  *m_testWidget;
    State                  m_state;
    DoubleTestPic          m_doubleTest;
};

}
}


#endif // DOUTESTWIDGET_H
