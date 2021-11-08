/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

    /**
     * @brief AppBody::resizeHintHeight 重新计算高度，直到title和body的达到最小行数或者高度不大于设定的理想值
     * @param idealHeight 理想高度
     * @return 最终高度（idealHeight和sizeHint取较大值）
     */
    int resizeHintHeight(const int idealHeight);

private:
    void refreshTheme();

private:
    AppBodyLabel *m_titleLbl;
    AppBodyLabel *m_bodyLbl;
    OSD::ShowStyle m_showStyle;
};

#endif // APPBODY_H
