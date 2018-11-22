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

#ifndef FONTSIZEWIDGET_H
#define FONTSIZEWIDGET_H

#include "widgets/translucentframe.h"
#include "widgets/settingsgroup.h"

#include <QObject>
#include <QSlider>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
class SettingsGroup;
class TitledSliderItem;
}
namespace personalization
{
class FontSizeModel;
class FontSizeWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSizeWidget(QWidget *parent = 0);

Q_SIGNALS:
    void requestSetFontSize(const int value);

public Q_SLOTS:
    void setModel(FontSizeModel *const model);
    void setFontSize(int size);

private:
    int delayToSliderValue(const int delay) const;
    void setFontSizeTip(int size);
    QString delayToLiteralString(const int delay) const;

private:
    QVBoxLayout   *m_mainlayout;
    widgets::SettingsGroup *m_mainWidget;
    widgets::TitledSliderItem *m_sizeWidget;
    QSlider          *m_fontSlider;
};
}
}

#endif // FONTSIZEWIDGET_H
