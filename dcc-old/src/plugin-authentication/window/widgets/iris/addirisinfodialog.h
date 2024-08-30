//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/titlelabel.h"
#include "widgets/buttontuple.h"
#include "widgets/disclaimersitem.h"
#include "widgets/iris/irisinfowidget.h"

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

// 虹膜对话框
class CharaMangerModel;
class AddIrisInfoDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:

    explicit AddIrisInfoDialog(CharaMangerModel *model, QWidget *parent = nullptr);
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
    void refreshInfoStatusDisplay(CharaMangerModel::AddInfoState state);
    void onSetWindowEnabled(const bool isEnabled);
    void refreshExplainTips(QString tips);

private:
    CharaMangerModel *m_charaModel;
    QVBoxLayout *m_mainLayout;
    IrisInfoWidget *m_irisInfo;
    QLabel *m_resultTips; // 录入结果说明
    QLabel *m_explainTips; // 状态说明信息
    DisclaimersItem *m_disclaimersItem; // 免责声明
    QPushButton* m_cancelBtn; // 取消
    DTK_WIDGET_NAMESPACE::DSuggestButton* m_acceptBtn; // 下一步
    CharaMangerModel::AddInfoState m_state;
};

