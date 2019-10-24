#include "systemrestore.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProcess>
#include <QFile>
#include <QSettings>
#include <QDebug>

#include <DDialog>
#include <DDBusSender>
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

DWIDGET_USE_NAMESPACE

SystemRestore::SystemRestore(QWidget *parent)
    : QWidget(parent)
{
    m_restoreAllSettingBtn = new QPushButton;
    m_restoreAllSettingAndDataBtn = new QPushButton;
    QLabel *restoreAllSettingTip = new QLabel;
    QLabel *restoreAllSettingAndDataTip = new QLabel;

    restoreAllSettingTip->setText(tr("Initialize all data on your system partition and reset the system configuration file, but keep the username, password, and the contents of other partitions."));
    restoreAllSettingAndDataTip->setText(tr("Reinstall the system, which will erase all data from the disk"));
    restoreAllSettingTip->setWordWrap(true);
    restoreAllSettingAndDataTip->setWordWrap(true);

    m_restoreAllSettingBtn->setText(tr("Reset All Settings"));
    m_restoreAllSettingAndDataBtn->setText(tr("Erase All Content and Settings"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_restoreAllSettingBtn);
    mainLayout->addWidget(restoreAllSettingTip);
    mainLayout->addWidget(m_restoreAllSettingAndDataBtn);
    mainLayout->addWidget(restoreAllSettingAndDataTip);
    mainLayout->addStretch();

    setLayout(mainLayout);

    connect(m_restoreAllSettingBtn, &QPushButton::clicked, this, [ = ] {
        writeRestoreConfig(false);
    });

    connect(m_restoreAllSettingAndDataBtn, &QPushButton::clicked, this, [ = ] {
        writeRestoreConfig(true);
    });
}

void SystemRestore::writeRestoreConfig(bool formatData) const
{
    QProcess process;
    process.start("pkexec", QStringList() << "/bin/restore-tool" << (formatData ? "--formatData" : ""));
    process.waitForFinished();

    if (process.exitStatus() != QProcess::NormalExit) {
        return;
    }

    DDialog dialog;
    QString message{ tr(
                         "This will reset all system settings to their defaults. Your data, username and "
                         "password will not be deleted, please confirm and continue") };

    if (formatData) {
        message =
            tr("This will reinstall the system and clear all user data. It is risky, "
               "please confirm and continue");
    }

    dialog.setMessage(message);
    dialog.addButton(tr("Cancel"));

    {
        int result = dialog.addButton(tr("Confirm"), true, DDialog::ButtonWarning);
        if (dialog.exec() != result) {
            return;
        }
    }

    DDialog reboot;

    if (formatData) {
        reboot.setMessage(tr("You should reboot the computer to erase all content and settings, reboot now?"));
    }
    else {
        reboot.setMessage(tr("You should reboot the computer to reset all settings, reboot now?"));
    }

    reboot.addButton("Cancel");
    {
        int result = reboot.addButton("Confirm", true, DDialog::ButtonWarning);
        if (reboot.exec() != result) {
            return;
        }
    }

    DDBusSender()
    .service("com.deepin.dde.shutdownFront")
    .path("/com/deepin/dde/shutdownFront")
    .interface("com.deepin.dde.shutdownFront")
    .method("Restart")
    .call();
}
