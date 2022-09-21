// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "modules/authentication/widgets/disclaimersdialog.h"
#include "modules/authentication/widgets/disclaimersitem.h"

#include <DAbstractDialog>
#include <DSuggestButton>

namespace DCC_NAMESPACE {
namespace authentication {

/**
 * @brief 生物特征采集弹窗的基类，公共的特性可以抽离出来放在这个类中
 *
 */
class BiologicalBaseDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit BiologicalBaseDialog(QWidget *parent = nullptr);
    ~BiologicalBaseDialog();

public slots:
    void onShowDisclaimer();
    void onAcceptDisclaimer(bool accept);

protected:
    void setDisclaimerVisible(bool visible);

protected:
    QWidget *m_bioWidget;
    dcc::authentication::DisclaimersDialog *m_disclaimerWidget;
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton *m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_acceptBtn; // 下一步
};

}
}
