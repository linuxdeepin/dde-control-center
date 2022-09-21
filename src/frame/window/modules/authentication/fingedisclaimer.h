// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FINGEDISCLAIMER_H
#define FINGEDISCLAIMER_H

#pragma once

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "modules/authentication/charamangermodel.h"
#include "biologicalbasedialog.h"

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

class FingerDisclaimer : public BiologicalBaseDialog
{
    Q_OBJECT
public:
    explicit FingerDisclaimer(QWidget *parent = nullptr);
    ~FingerDisclaimer();

Q_SIGNALS:
    void requestShowFingeInfoDialog();
    void requestStopEnroll();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void initWidget();
    void initBioWidget();
    void initConnect();
    QString getFingerPicture();

private:
    QLabel *m_fingerPic;
    QLabel *m_resultTips; // 录入结果说明
    DLabel *m_explainTips; // 状态说明信息
    dcc::authentication::CharaMangerModel::AddInfoState m_currentState;
};

}
}


#endif //FINGEDISCLAIMER_H
