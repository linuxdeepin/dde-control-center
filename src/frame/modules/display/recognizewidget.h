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

#ifndef RECOGNIZEWIDGET_H
#define RECOGNIZEWIDGET_H

#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class RecognizeWidget : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit RecognizeWidget(Monitor *monitor, QString name, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void onScreenRectChanged();

private:
    Monitor *m_monitor;
    QRect m_rect;
    QString m_text;
};

} // namespace display

} // namespace dcc

#endif // RECOGNIZEWIDGET_H
