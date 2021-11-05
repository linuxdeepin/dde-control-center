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

    explicit AddIrisInfoDialog(dcc::authentication::CharaMangerModel *model, DAbstractDialog *parent = nullptr);
    ~AddIrisInfoDialog();

Q_SIGNALS:
    void requestInputIris();
    void requesetCloseDlg();
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

private:
    dcc::authentication::CharaMangerModel *m_charaModel;
    QVBoxLayout *m_mainLayout;
    dcc::authentication::IrisInfoWidget *m_irisInfo;
    QLabel *m_resultTips; // 录入结果说明
    QLabel *m_explainTips; // 状态说明信息
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
};

}
}
