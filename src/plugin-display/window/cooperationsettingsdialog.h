/*
 * Copyright (C) 2011 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef COOPERATIONSETTINGSDIALOG_H
#define COOPERATIONSETTINGSDIALOG_H

#include "filechoosewidget.h"
#include "interface/namespace.h"
#include "widgets/titlelabel.h"

#include <DAbstractDialog>
#include <DSuggestButton>
#include <DTipLabel>
#include <buttontuple.h>
#include <switchwidget.h>

QT_BEGIN_NAMESPACE
class QDialog;
class QVBoxLayout;
QT_END_NAMESPACE

// 协同设置
class CooperationSettingsDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit CooperationSettingsDialog(QWidget *parent = nullptr);
    ~CooperationSettingsDialog();

    void onSetWindowEnabled(const bool isEnabled);

    DCC_NAMESPACE::SwitchWidget *mousekeyboardSwitch() { return m_mousekeyboardSwitch; }
    DCC_NAMESPACE::SwitchWidget *shearClipboardSwitch() { return m_shearSwitch; }
    DCC_NAMESPACE::ButtonTuple *buttonTuple() { return m_buttonTuple; }
    const QString& storagePath() { return m_storagePath; }

Q_SIGNALS:
    void requestFilesStoragePath(const QString &path);

public Q_SLOTS:
    void setOpenSharedDevices(bool on);
    void setOpenSharedClipboard(bool on);
    void setFilesStoragePath(const QString& path);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void initWidget();
    void initConnect();
    void setButtonTupleState(bool state);

private:
    QVBoxLayout *m_mainLayout;
    DCC_NAMESPACE::SwitchWidget *m_mousekeyboardSwitch;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_mousekeyboardTips;

    DCC_NAMESPACE::SwitchWidget *m_shearSwitch; // 剪切板共享

    FileChooseWidget *m_storageItem; // 共享存储
    DTK_WIDGET_NAMESPACE::DTipLabel *m_storageTips;

    DCC_NAMESPACE::ButtonTuple *m_buttonTuple;
    QString m_storagePath;
};

#endif // COOPERATIONSETTINGSDIALOG_H
