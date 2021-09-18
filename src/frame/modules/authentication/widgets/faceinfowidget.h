/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "modules/authentication/charamangermodel.h"

#include <dareader/reader.h>

#include <QLabel>
#include <QDBusUnixFileDescriptor>

namespace dcc {
namespace authentication {

class FaceInfoWidget : public QLabel
{
    Q_OBJECT
public:
    explicit FaceInfoWidget(QWidget *parent = nullptr);
    ~FaceInfoWidget();

    void initWidget();
    void createConnection(const int fd);

private Q_SLOTS:
    /**
     * @brief onUpdateProgressbar 刷新外圈进度条
     */
    void onUpdateProgressbar();

private:
    void paintEvent(QPaintEvent *event);
    static void recvCamara(void *const context, const DA_img *const img);

private:
    DA_img *m_videoData;
    QLabel *m_faceLable;
    QTimer *m_startTimer;
    QColor m_themeColor;

    int m_persent; // 记录进度
    int m_rotateAngle;//旋转角度
    int m_postRotateAngle;
};

}
}
