/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#ifndef PAPERSETTINGWIDGET_H
#define PAPERSETTINGWIDGET_H

#include "interface/namespace.h"

#include <DWidget>

namespace dcc {
namespace personalization {
class PersonalizationModel;
class PersonalizationWork;
}
}

class QStackedLayout;

namespace DCC_NAMESPACE {
namespace personalization {

class PaperDetailWidget;
class WallpaperView;

class PaperSettingWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit PaperSettingWidget(dcc::personalization::PersonalizationWork *work
                                , dcc::personalization::PersonalizationModel *model
                                , Dtk::Widget::DWidget *parent = nullptr);

Q_SIGNALS:
    void requestExit();

private Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void onCancelClicked();
    void updateView();

private:
    void initConnect();

private:
    QStackedLayout *m_mainLayout;
    PaperDetailWidget *m_settingPage;
    WallpaperView *m_paperView;
    dcc::personalization::PersonalizationWork *m_work;
    dcc::personalization::PersonalizationModel *m_model;
};
}
}

#endif // PAPERSETTINGWIDGET_H
