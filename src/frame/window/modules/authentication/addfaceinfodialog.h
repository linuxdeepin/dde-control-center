// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "modules/authentication/widgets/disclaimersitem.h"
#include "modules/authentication/charamangermodel.h"

#include <DSuggestButton>
#include <DAbstractDialog>
#include <DTipLabel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
class QCloseEvent;
class QDialog;
QT_END_NAMESPACE

namespace dcc {
namespace authentication {
class CharaMangerModel;
}
}

using namespace dcc::widgets;


namespace DCC_NAMESPACE {
namespace authentication {

// 添加人脸对话框
class AddFaceInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFaceInfoDialog(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~AddFaceInfoDialog();

    void onSetWindowEnabled(const bool isEnabled);
    /**
     * @brief responseEnrollInfoState 响应当前人脸录入状态
     * @param state 三种状态
     * @param tips 对应的提示语
     */
    void responseEnrollInfoState(dcc::authentication::CharaMangerModel::AddInfoState state, const QString &tips);

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
    dcc::authentication::CharaMangerModel *m_faceModel;
    QVBoxLayout *m_mainLayout;
    QLabel *m_facePic; // 人脸图片
    QLabel *m_resultTips; // 录入结果说明
    DLabel *m_explainTips; // 状态说明信息
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
    dcc::authentication::CharaMangerModel::AddInfoState m_currentState;
};

}
}
