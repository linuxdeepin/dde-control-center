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

#ifndef SYSTEMINFOWIDGET_H
#define SYSTEMINFOWIDGET_H

#include "modules/modulewidget.h"
#include "systeminfomodel.h"

using namespace dcc;

namespace dcc
{

namespace widgets {

class TitleValueItem;
class NextPageWidget;

}

namespace systeminfo
{

class SystemInfoWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit SystemInfoWidget(SystemInfoModel* model);

Q_SIGNALS:
    void copyright();
    void boot();
    void requestShowLicense();

public Q_SLOTS:
    void setEdition(const QString& edition);
    void setType(const QString& type);
    void setProcessor(const QString& processor);
    void setMemory(const QString& memory);
    void setDistroID(const QString& distroid);
    void setDistroVer(const QString& distrover);
    void setDisk(const QString& disk);

private:
    widgets::TitleValueItem* m_distroid;
    widgets::TitleValueItem* m_distrover;
    widgets::TitleValueItem* m_version;
    widgets::TitleValueItem* m_type;
    widgets::TitleValueItem* m_processor;
    widgets::TitleValueItem* m_memory;
    widgets::TitleValueItem* m_disk;
    widgets::NextPageWidget* m_copyright;

    widgets::NextPageWidget* m_boot;
    SystemInfoModel* m_model;
};

}
}
#endif // SYSTEMINFOWIDGET_H
