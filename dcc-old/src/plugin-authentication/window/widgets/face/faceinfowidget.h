//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "charamangermodel.h"
#include <dareader/reader.h>

#include <QLabel>

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
