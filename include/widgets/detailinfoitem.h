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

inline const QString titleColor = QStringLiteral("#0082fa");
inline const QString grayColor = QStringLiteral("#526a7f");

inline constexpr int titleFontSize = 14;
inline constexpr int dateFontSize = 12;
inline constexpr int detailFontSize = 8;
inline constexpr int linkFontSize = 10;

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

private Q_SLOTS:
    void onThemeChanged();

private:
    Dtk::Widget::DLabel *m_dateLabel;
    Dtk::Widget::DLabel *m_explainTitle;
    Dtk::Widget::DLabel *m_linkDataLabel;
    Dtk::Widget::DLabel *m_dataLable;
    Dtk::Widget::DLabel *m_linkLable;
    Dtk::Widget::DLabel *m_title;
};

}

