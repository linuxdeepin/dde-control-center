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
#include "widgets/titlelabel.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCloseEvent;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

//添加指纹对话框
class AddFingeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit AddFingeDialog(const QString &thumb, DAbstractDialog *parent = nullptr);
    ~AddFingeDialog() override;

    void setFingerModel(dcc::accounts::FingerModel *model);
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

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestStopEnroll(const QString &thumb);
    void requestReEnrollThumb();
    void requestEnrollThumb();
    void requesetCloseDlg(const QString &name);

private:
    QTimer *m_timer;
    dcc::accounts::FingerModel *m_model;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleHLayout;
    QHBoxLayout *m_btnHLayout;
    dcc::accounts::FingerWidget *m_fingeWidget;
    QString m_thumb;
    QString m_username;
    QPushButton *m_cancelBtn;
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_addBtn;
    bool m_isEnrolling{true};
};

}
}
