// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "modules/authentication/charamangermodel.h"
#include <modules/authentication/widgets/irisinfowidget.h>
#include "biologicalbasedialog.h"

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
// 虹膜对话框
class AddIrisInfoDialog : public BiologicalBaseDialog
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
    void initBioWidget();

public Q_SLOTS:
    void refreshInfoStatusDisplay(dcc::authentication::CharaMangerModel::AddInfoState state);
    void onSetWindowEnabled(const bool isEnabled);
    void refreshExplainTips(QString tips);

private:
    dcc::authentication::CharaMangerModel *m_charaModel;
    dcc::authentication::IrisInfoWidget *m_irisInfo;
    QLabel *m_resultTips; // 录入结果说明
    QLabel *m_explainTips; // 状态说明信息
    QWidget *m_spaceWidget;
    dcc::authentication::CharaMangerModel::AddInfoState m_state;
};

}
}
