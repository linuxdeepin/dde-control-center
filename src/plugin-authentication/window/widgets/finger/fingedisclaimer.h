//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "charamangermodel.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "widgets/disclaimersitem.h"

#include <DAbstractDialog>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DTipLabel;
DWIDGET_END_NAMESPACE

class CharaMangerModel;
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
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *o, QEvent *e);

private:
    void initWidget();
    void initConnect();
    QString getFacePicture();

private:
    QVBoxLayout *m_mainLayout;
    QLabel *m_fingerPic;
    QLabel *m_resultTips; // 录入结果说明
    DTK_WIDGET_NAMESPACE::DLabel *m_explainTips; // 状态说明信息
    DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton *m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_acceptBtn; // 下一步
    CharaMangerModel::AddInfoState m_currentState;
};
