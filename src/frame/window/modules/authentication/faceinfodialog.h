// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/authentication/charamangermodel.h"
#include "modules/authentication/widgets/faceinfowidget.h"

#include <DSuggestButton>
#include <DAbstractDialog>
#include <DTipLabel>

#include <QObject>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
class QCloseEvent;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace authentication {
class FaceInfoWidget;
}
}

namespace DCC_NAMESPACE {
namespace authentication {

// 人脸视频录入对话框
class FaceInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit FaceInfoDialog(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~FaceInfoDialog();

    inline dcc::authentication::FaceInfoWidget* faceInfoLabel() { return m_faceLabel; }

private:
    void initWidget();

private Q_SLOTS:
    void refreshExplainTips(QString tips);

Q_SIGNALS:
    void requestCloseDlg();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    dcc::authentication::CharaMangerModel *m_faceModel;
    dcc::authentication::FaceInfoWidget *m_faceLabel;

    QVBoxLayout *m_mainLayout;
    QLabel *m_explainTips; // 状态说明信息
};

}
}
