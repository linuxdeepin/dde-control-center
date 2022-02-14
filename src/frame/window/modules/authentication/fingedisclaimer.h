#ifndef FINGEDISCLAIMER_H
#define FINGEDISCLAIMER_H

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

class FingerDisclaimer : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit FingerDisclaimer(QWidget *parent = nullptr);
    ~FingerDisclaimer();

    void onSetWindowEnabled(const bool isEnabled);

Q_SIGNALS:
    void requestShowFingeInfoDialog();
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
    QVBoxLayout *m_mainLayout;
    QLabel *m_fingerPic;
    QLabel *m_resultTips; // 录入结果说明
    DLabel *m_explainTips; // 状态说明信息
    dcc::authentication::DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton *m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_acceptBtn; // 下一步
    dcc::authentication::CharaMangerModel::AddInfoState m_currentState;
};

}
}


#endif //FINGEDISCLAIMER_H
