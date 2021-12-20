/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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
    explicit AddFaceInfoDialog(dcc::authentication::CharaMangerModel *model, DAbstractDialog *parent = nullptr);
    ~AddFaceInfoDialog();

    void onSetWindowEnabled(const bool isEnabled);

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

    /**
     * @brief responseEnrollInfoState 响应当前人脸录入状态
     * @param state 三种状态
     * @param tips 对应的提示语
     */
    void responseEnrollInfoState(dcc::authentication::CharaMangerModel::AddInfoState state, const QString &tips);

private:
    dcc::authentication::CharaMangerModel *m_faceModel;
    QVBoxLayout *m_mainLayout;
    QLabel *m_facePic; // 人脸图片
    QLabel *m_resultTips; // 录入结果说明
    QLabel *m_explainTips; // 状态说明信息
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
    dcc::authentication::CharaMangerModel::AddInfoState m_currentState;
};

}
}
