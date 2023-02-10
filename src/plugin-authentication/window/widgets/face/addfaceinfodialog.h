//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "widgets/disclaimersitem.h"
#include "charamangermodel.h"

#include <DAbstractDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
class QCloseEvent;
class QDialog;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DTipLabel;
DWIDGET_END_NAMESPACE

// 添加人脸对话框
class CharaMangerModel;
class AddFaceInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFaceInfoDialog(CharaMangerModel *model, QWidget *parent = nullptr);
    ~AddFaceInfoDialog();

    void onSetWindowEnabled(const bool isEnabled);
    /**
     * @brief responseEnrollInfoState 响应当前人脸录入状态
     * @param state 三种状态
     * @param tips 对应的提示语
     */
    void responseEnrollInfoState(CharaMangerModel::AddInfoState state, const QString &tips);

Q_SIGNALS:
    void requestShowFaceInfoDialog();
    void requesetCloseDlg();
    void requestStopEnroll();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void initWidget();
    void initConnect();
    QString getFacePicture();


private:
    CharaMangerModel *m_faceModel;
    QVBoxLayout *m_mainLayout;
    QLabel *m_facePic; // 人脸图片
    QLabel *m_resultTips; // 录入结果说明
    DLabel *m_explainTips; // 状态说明信息
    DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
    CharaMangerModel::AddInfoState m_currentState;
};

