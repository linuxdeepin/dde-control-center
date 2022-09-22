// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPBODY_H
#define APPBODY_H

#include "constants.h"
#include <QWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class AppBodyLabel;
/*!
 * \~chinese \class AppBody
 * \~chinese \brief 展示通知气泡的Body部分信息的窗口
 */
class AppBody : public QFrame
{
    Q_OBJECT

public:
    explicit AppBody(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void setText(const QString &text);
    void setStyle(OSD::ShowStyle style);
    static int bubbleWidgetAppBodyHeight();
    int bubbleWindowAppBodyHeight();

private:
    void refreshTheme();

private:
    AppBodyLabel *m_titleLbl;
    AppBodyLabel *m_bodyLbl;
    OSD::ShowStyle m_showStyle;
};

#endif // APPBODY_H
