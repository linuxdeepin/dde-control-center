// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    static bool EnableRecvImage;

    int m_persent; // 记录进度
    int m_rotateAngle;//旋转角度
    int m_postRotateAngle;
};

}
}
