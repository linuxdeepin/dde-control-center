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

#include <modules/authentication/widgets/irisinfowidget.h>

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
// 虹膜对话框
class AddIrisInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:

    explicit AddIrisInfoDialog(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~AddIrisInfoDialog();

Q_SIGNALS:
    void requestInputIris();
    void requestCloseDlg();
    void requestStopEnroll();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void initWidget();
    void initConnect();

public Q_SLOTS:
    void refreshInfoStatusDisplay(dcc::authentication::CharaMangerModel::AddInfoState state);
    void onSetWindowEnabled(const bool isEnabled);
    void refreshExplainTips(QString tips);

private:
    dcc::authentication::CharaMangerModel *m_charaModel;
    QVBoxLayout *m_mainLayout;
    dcc::authentication::IrisInfoWidget *m_irisInfo;
    QLabel *m_resultTips; // 录入结果说明
    QLabel *m_explainTips; // 状态说明信息
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
    dcc::authentication::CharaMangerModel::AddInfoState m_state;
};

}
}
