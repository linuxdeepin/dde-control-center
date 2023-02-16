//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

