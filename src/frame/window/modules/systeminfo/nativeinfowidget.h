/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "interface/namespace.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "widgets/titlevalueitem.h"
#include "widgets/contentwidget.h"

#include <QWidget>

using namespace dcc;

QT_BEGIN_NAMESPACE

class QVBoxLayout;

QT_END_NAMESPACE

namespace dcc {
namespace systeminfo {
class SystemInfoModel;
}
}

namespace DCC_NAMESPACE {
namespace systeminfo {

class NativeInfoWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit NativeInfoWidget(dcc::systeminfo::SystemInfoModel *model, QWidget *parent = nullptr);

private:
    void initWidget();
    const QString systemCopyright() const;
    const QString systemLogo() const;

public Q_SLOTS:
    void setProductName(const QString &edition);
    void setVersionNumber(const QString &type);
    void setEdition(const QString &edition);
    void setType(const QString &type);
    void setProcessor(const QString &processor);
    void setMemory(const QString &memory);
    void setLicenseState(quint32 state);

Q_SIGNALS:
    void clickedActivator();

private:
    dcc::systeminfo::SystemInfoModel *m_model;
    QVBoxLayout *m_mainLayout;
    dcc::widgets::TitleValueItem *m_productName;
    dcc::widgets::TitleValueItem *m_versionNumber;
    dcc::widgets::TitleValueItem *m_version;
    dcc::widgets::TitleValueItem *m_type;
    dcc::widgets::TitleAuthorizedItem *m_authorized;
    dcc::widgets::TitleValueItem *m_kernel;
    dcc::widgets::TitleValueItem *m_processor;
    dcc::widgets::TitleValueItem *m_memory;
};

}
}
