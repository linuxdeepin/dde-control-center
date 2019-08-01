/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#pragma once

#include "window/namespace.h"

#include <QObject>
#include <QWidget>
#include <QListView>

#include <dimagebutton.h>

class QStackedLayout;
class QVBoxLayout;

namespace dcc {
    namespace update {
        class UpdateModel;
        class UpdateWorker;
    }
}

namespace DCC_NAMESPACE {
namespace update {

const unsigned int default_listview_index = 0;
static const QString OfflineUpgraderService = "com.deepin.dde.OfflineUpgrader";

class UpdateCtrlWidget;

class UpdateWidget : public QWidget
{
    Q_OBJECT
    enum updateType {
        DEFAULT = -1,
        CHECK_UPDATE,
        UPDATE_SETTING,
        COUNT
    };
public:
    explicit UpdateWidget(QWidget *parent = nullptr);
    ~UpdateWidget();

    void initialize();   
    void setModel(const dcc::update::UpdateModel *model, const dcc::update::UpdateWorker *work);
    void setDefaultState();

private:
    void showCheckUpdate();
    void showUpdateSetting();

Q_SIGNALS:
    void topListviewChanged(const QModelIndex &index);
    void pushMirrorsView();

public Q_SLOTS:
    void onTopListviewCliecked(const QModelIndex &index);

private:
    Dtk::Widget::DImageButton *m_bottomLabel;
    QVBoxLayout *m_layout;
    QListView *m_listview;
    QModelIndex m_defaultIndex;
    dcc::update::UpdateModel* m_model;
    dcc::update::UpdateWorker* m_work;
    QStackedLayout *m_centerLayout;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
