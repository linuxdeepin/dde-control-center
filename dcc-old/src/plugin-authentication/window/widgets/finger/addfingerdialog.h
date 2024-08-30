//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "fingerinfowidget.h"
#include "widgets/titlelabel.h"

#include <DAbstractDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
class QDialog;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DTipLabel;
DWIDGET_END_NAMESPACE

//添加指纹对话框
class CharaMangerModel;
class AddFingerDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFingerDialog(const QString &thumb, QWidget *parent = nullptr);
    ~AddFingerDialog() override;

    void setFingerModel(CharaMangerModel *model);
    void setUsername(const QString &name);
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollFailed(QString title, QString msg);
    void setInitStatus();
    void enrollDisconnected();
    void enrollFocusOut();
    void enrollOverTime();
    void enrollRetry(QString title, QString msg);

private:
    void initWidget();
    void initData();
    void keyPressEvent(QKeyEvent *event);
protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *o, QEvent *e);
Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestStopEnroll(const QString &thumb);
    void requestReEnrollThumb();
    void requestEnrollThumb();
    void requesetCloseDlg(const QString &name);

private:
    QTimer *m_timer;
    CharaMangerModel *m_model;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_btnHLayout;
    FingerInfoWidget *m_fingeWidget;
    QString m_thumb;
    QString m_username;
    QPushButton *m_cancelBtn;
    QWidget *m_spaceWidget;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_addBtn;
    bool m_isEnrolling{true};
};
