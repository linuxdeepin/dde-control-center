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

#ifndef FONTSETTINGSWIDGET_H
#define FONTSETTINGSWIDGET_H

#include "widgets/translucentframe.h"

#include <QObject>
#include <QVBoxLayout>
#include <QDebug>

namespace dcc
{
namespace widgets
{
class SettingsGroup;
class NextPageWidget;
}
namespace personalization
{
class PersonalizationModel;
class FontSettingsWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSettingsWidget(QWidget *parent = 0);
    void setModel(PersonalizationModel *const model);

Q_SIGNALS:
    void showStandardFont();
    void showMonoFont();

private Q_SLOTS:
    void onStandFontChanged(const QString &name);
    void onMonoFontChanged(const QString &name);

private:
    QVBoxLayout    *m_mainlayout;
    widgets::SettingsGroup  *m_fontWidget;
    widgets::NextPageWidget *m_standard;
    widgets::NextPageWidget *m_mono;
    PersonalizationModel *m_model;
};
}
}


#endif // FONTSETTINGSWIDGET_H
