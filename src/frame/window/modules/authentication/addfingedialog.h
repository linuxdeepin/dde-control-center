// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/authentication/widgets/fingerwidget.h"

#include <DSuggestButton>
#include <DAbstractDialog>
#include <DTipLabel>
#include "widgets/titlelabel.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
QT_END_NAMESPACE

namespace dcc {
namespace authentication {
class FingerModel;
}
}

namespace DCC_NAMESPACE {
namespace authentication {

//添加指纹对话框
class AddFingeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFingeDialog(const QString &thumb, DAbstractDialog *parent = nullptr);
    ~AddFingeDialog() override;

    void setFingerModel(dcc::authentication::FingerModel *model);
    void setUsername(const QString &name);
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollFailed(QString title, QString msg);
    void setInitStatus();
    void enrollDisconnected();
    void enrollFocusOut();
    void enrollOverTime();
    void enrollRetry(QString title, QString msg);
    void claimFailed();

private:
    void initWidget();
    void initData();
    void keyPressEvent(QKeyEvent *event) override;
protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestStopEnroll(const QString &thumb);
    void requestReEnrollThumb();
    void requestEnrollThumb();
    void requestCloseDlg(const QString &name);

private:
    QTimer *m_timer;
    dcc::authentication::FingerModel *m_model;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_btnHLayout;
    dcc::authentication::FingerWidget *m_fingeWidget;
    QString m_thumb;
    QString m_username;
    QPushButton *m_cancelBtn;
    QWidget *m_spaceWidget;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_addBtn;
    bool m_isEnrolling{true};
};

}
}
