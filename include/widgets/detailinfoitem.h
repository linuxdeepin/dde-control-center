/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     shenwenqi <shenwenqi@uniontech.com>

 * Maintainer: shenwenqi <shenwenqi@uniontech.com>
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
#include "widgets/settingsitem.h"
#include <dtkwidget_global.h>

DWIDGET_BEGIN_NAMESPACE
class DLabel;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {

const QString titleColor = "#0082fa";
const QString grayColor = "#526a7f";

const int titleFontSize = 14;
const int dateFontSize = 12;
const int detailFontSize = 8;
const int linkFontSize = 10;

class DetailInfoItem: public SettingsItem
{
    Q_OBJECT
public:
    explicit DetailInfoItem(QWidget *parent = 0);

    void initUi();

    void setDate(QString date);
    void setTitle(QString title);
    void setExplaintTitle(QString title);
    void setLinkData(QString data);
    void setDetailData(QString data);

private:
    Dtk::Widget::DLabel *m_dateLabel;
    Dtk::Widget::DLabel *m_explainTitle;
    Dtk::Widget::DLabel *m_linkDataLabel;
    Dtk::Widget::DLabel *m_dataLable;
    Dtk::Widget::DLabel *m_linkLable;
    Dtk::Widget::DLabel *m_title;
};

}

