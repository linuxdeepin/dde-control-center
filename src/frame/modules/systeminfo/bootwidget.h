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

#ifndef BOOTWIDGET_H
#define BOOTWIDGET_H

#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"

#include <QListWidget>

using namespace dcc::widgets;

namespace dcc{
namespace systeminfo{

class SystemInfoModel;
class GrubBackgroundItem;
class BootWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit BootWidget(QWidget* parent = 0);

    void setDefaultEntry(const QString& value);
    void setModel(SystemInfoModel *model);

Q_SIGNALS:
    void enableTheme(bool value);
    void bootdelay(bool value);
    void defaultEntry(const QString& item);
    void requestSetBackground(const QString &path);

public Q_SLOTS:
    void setEntryList(const QStringList& list);
    void onCurrentItem(QListWidgetItem* cur, QListWidgetItem* pre);

private:
    QString m_defaultEntry;
    SwitchWidget* m_boot;
    SwitchWidget* m_theme;
    QListWidget* m_bootList;
    TipsLabel *m_updatingLabel;
    GrubBackgroundItem *m_background;
};

}
}
#endif // BOOTWIDGET_H
