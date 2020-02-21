/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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
#include "modules/accounts/fingermodel.h"
#include "modules/accounts/widgets/fingerwidget.h"

#include <DSuggestButton>
#include <DAbstractDialog>
#include <DTipLabel>

#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

enum enrollStage{
  enrollFirstStage = 1,
  enrollSecondStage
};
//添加指纹对话框
class AddFingeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFingeDialog(const QString &thumb, DAbstractDialog *parent = nullptr);
    void setFingerModel(dcc::accounts::FingerModel *model);
    void setUsername(const QString &name);
    void setDefaultTitleTip();
    void setDefaultMsgTip();
    void enrollCompleted();
    void enrollStagePass(int pro);
    void enrollFailed(QString msg);
    void setInitStatus();
    void enrollDisconnected();
    void enrollRetry(QString msg);

private:
    void initWidget();
    void initData();

protected:
    void closeEvent(QCloseEvent *event);

Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestReEnrollStart(const QString &thumb);
    void requestStopEnroll();
    void requestReEnrollThumb();
    void requestEnrollThumb();

public Q_SLOTS:
    void onViewPlayEnd();

private:
    dcc::accounts::FingerModel *m_model;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleHLayout;
    QVBoxLayout *m_contentVLayout;
    QVBoxLayout *m_fingerHLayout;
    QHBoxLayout *m_btnHLayout;
    dcc::accounts::FingerWidget *m_fingeWidget;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_scanBtn;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_doneBtn;
    QString m_thumb;
    QString m_username;
    QPushButton *m_cancelBtn;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_addBtn;
    DTipLabel *m_titleTip;
    QTimer *m_qtimerTitleTip;
    QTimer *m_qtimerMsgTip;
    enrollStage m_enrollStage{enrollStage::enrollFirstStage};
};

}
}
