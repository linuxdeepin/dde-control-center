/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#ifndef RESOLUTIONDETAILPAGE_H_V20
#define RESOLUTIONDETAILPAGE_H_V20

#include "namespace.h"
#include "widgets/contentwidget.h"

#include <DListView>

#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

class Resolution;

namespace dcc {

namespace widgets {
class SettingsGroup;
}

namespace display {

class Monitor;
class DisplayModel;
}

}

namespace DCC_NAMESPACE {

namespace display {

class ResolutionDetailPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit ResolutionDetailPage(QWidget *parent = nullptr);

    enum ResolutionRole {
        IdRole = Dtk::UserRole,
        WidthRole,
        HeightRole,
        RateRole
    };

public:
    void setModel(dcc::display::DisplayModel *mode);

Q_SIGNALS:
    void requestSetResolution(dcc::display::Monitor *, int);
    void requestReset();
    void requestSave();

private Q_SLOTS:
    void initResoList();

private:
    QVBoxLayout *m_mainLayout{nullptr};
    dcc::display::DisplayModel *m_model{nullptr};

    DListView *m_resoList{nullptr};
    QModelIndex m_curIdxs;
};

}   // namespace dcc

}   // namespace display

#endif // RESOLUTIONDETAILPAGE_H_V20
