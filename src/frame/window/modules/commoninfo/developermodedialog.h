// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DeveloperModeDialog_H
#define DeveloperModeDialog_H

#include "interface/namespace.h"

#include <DAbstractDialog>
#include <DRadioButton>
#include <DSuggestButton>

struct TS{
    QString ts1;
    QString ts2;
};

class QFileDialog;
namespace DCC_NAMESPACE {
namespace commoninfo {

class CommonInfoModel;

class DeveloperModeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit DeveloperModeDialog(QObject *parent = nullptr);
    ~DeveloperModeDialog();

public:
     void setModel(CommonInfoModel *model);
     void shutdown();

Q_SIGNALS:
     void requestDeveloperMode(bool enabled);
     void requestLogin();
     void requestCommit(QString filePathName);

protected:
     virtual void showEvent(QShowEvent *);

private Q_SLOTS:
     void setLogin();

private:
    DTK_WIDGET_NAMESPACE::DRadioButton *m_onlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DRadioButton *m_offlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_nextButton{nullptr};
    CommonInfoModel *m_model{nullptr};
    bool m_enterDev{false};
    QFileDialog *m_importFile;
    QFileDialog *m_exportFile;
};
}
}

#endif // DEVELPERMODEACTIVATE_H
