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
#ifndef DETAILINFOITEM_H
#define DETAILINFOITEM_H

#include "widgets/labels/normallabel.h"
#include "widgets/labels/largelabel.h"
#include "widgets/settingsitem.h"
#include "widgets/contentwidget.h"

#include <QWidget>
#include <QLabel>

#include <DFloatingButton>
#include <DCommandLinkButton>
#include <DLabel>
#include <DLineEdit>
#include <DTextEdit>
#include <DTipLabel>

namespace dcc {

namespace widgets {

const QString titleColor = "#0082fa";
const QString grayColor = "#526a7f";

const int titleFontSize = 14;
const int dateFontSize = 12;
const int detailFontSize = 8;
const int linkFontSize = 10;

class DetailInfoItem: public dcc::widgets::SettingsItem
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
    DLabel *m_dateLabel;
    DLabel *m_explainTitle;
    DLabel *m_linkDataLabel;
    DLabel *m_dataLable;
    DLabel *m_linkLable;
    DLabel *m_title;
};

}

}

#endif //DETAILINFOITEM_H
