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

#ifndef FONTSWIDGET_H
#define FONTSWIDGET_H

#include "widgets/contentwidget.h"
#include "widgets/translucentframe.h"

#include <QObject>
#include <QVBoxLayout>
#include <QDebug>
namespace dcc
{
class ContentWidget;
namespace widgets
{
class TranslucentFrame;
}
namespace personalization
{
class FontSizeWidget;
class FontSettingsWidget;
class PersonalizationModel;
class FontsWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit FontsWidget(QWidget *parent = 0);

Q_SIGNALS:
    void showStandardFont();
    void showMonoFont();
    void requestSetFontSize(const int value);

public Q_SLOTS:
    void setModel(PersonalizationModel *const model);

private:
    QVBoxLayout        *m_mainlayout;
    widgets::TranslucentFrame   *m_widget;
    FontSizeWidget     *m_fontSizeWidget;
    FontSettingsWidget *m_fontSettingsWidget;
};

}
}

#endif // FONTSWIDGET_H
