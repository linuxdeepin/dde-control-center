//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "charamangermodel.h"
#include "widgets/face/faceinfowidget.h"

#include <DAbstractDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
class QCloseEvent;
class QLabel;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DTipLabel;
DWIDGET_END_NAMESPACE

// 人脸视频录入对话框
class FaceInfoWidget;
class FaceInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit FaceInfoDialog(CharaMangerModel *model, QWidget *parent = nullptr);
    ~FaceInfoDialog();

    inline FaceInfoWidget* faceInfoLabel() { return m_faceLabel; }

private:
    void initWidget();

private Q_SLOTS:
    void refreshExplainTips(QString tips);

Q_SIGNALS:
    void requestCloseDlg();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    CharaMangerModel *m_faceModel;
    FaceInfoWidget *m_faceLabel;

    QVBoxLayout *m_mainLayout;
    QLabel *m_explainTips; // 状态说明信息
};
