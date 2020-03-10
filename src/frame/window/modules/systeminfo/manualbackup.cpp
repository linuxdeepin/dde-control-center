#include "manualbackup.h"

#include <QVBoxLayout>
#include <QLabel>
#include <DBackgroundGroup>
#include <QPushButton>
#include <QDir>
#include <QProcess>
#include <DDBusSender>
#include <QSharedPointer>

DCORE_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

ManualBackup::ManualBackup(QWidget* parent)
    : QWidget(parent)
    , m_directoryChooseWidget(new DFileChooserEdit)
    , m_tipsLabel(new QLabel(tr("Choose Directory invalid")))
    , m_backupBtn(new QPushButton(tr("Backup")))
    , m_grubInter(nullptr)
{
    m_tipsLabel->setWordWrap(true);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(5);

    QHBoxLayout* chooseLayout = new QHBoxLayout;
    chooseLayout->setMargin(0);
    chooseLayout->setSpacing(0);
    chooseLayout->addWidget(new QLabel(tr("Set backup directory")), 0, Qt::AlignVCenter);
    chooseLayout->addSpacing(5);
    chooseLayout->addWidget(m_directoryChooseWidget, 0, Qt::AlignVCenter);

    QWidget* bgWidget = new QWidget;
    bgWidget->setLayout(chooseLayout);

    QVBoxLayout* bgLayout = new QVBoxLayout;
    bgLayout->addWidget(bgWidget);

    DBackgroundGroup* bgGroup = new DBackgroundGroup;
    bgGroup->setLayout(bgLayout);
    bgGroup->setBackgroundRole(QPalette::Window);
    bgGroup->setItemSpacing(1);

    mainLayout->addWidget(bgGroup);
    mainLayout->addWidget(m_tipsLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(m_backupBtn);

    setLayout(mainLayout);

    m_tipsLabel->hide();
    m_backupBtn->setEnabled(false);

    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    m_directoryChooseWidget->setFileDialog(fileDialog);

    connect(m_directoryChooseWidget, &DFileChooserEdit::dialogClosed, this, &ManualBackup::onChoose);
    connect(m_backupBtn, &QPushButton::clicked, this, &ManualBackup::backup);
}

void ManualBackup::onChoose()
{
    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();
    m_backupBtn->setEnabled(false);
    if (choosePath.isEmpty()) {
        return;
    }

    if (!(choosePath.startsWith("/data") || choosePath.startsWith("/home"))) {
        m_tipsLabel->show();
        return;
    }

    m_tipsLabel->hide();

    m_backupBtn->setEnabled(!m_directoryChooseWidget->lineEdit()->text().isEmpty());
}

void ManualBackup::backup()
{
    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();

    QSharedPointer<QProcess> process(new QProcess);
    process->start("pkexec", QStringList() << "/bin/restore-tool" << "--actionType" << "manual_backup" << "--path" << choosePath);
    process->waitForFinished();

    if (process->exitCode() != 0) {
        return;
    }

    if (!m_grubInter) {
        m_grubInter = new GrubInter("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2", QDBusConnection::systemBus());
        m_grubInter->setSync(true, true);
    }

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(m_grubInter->SetDefaultEntry("Deepin Recovery"), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &ManualBackup::onGrubSetFinished);
}

void ManualBackup::onGrubSetFinished(QDBusPendingCallWatcher *self)
{
    if (!self->isError()) {
        const bool isEnable = m_backupBtn->isEnabled();
        m_backupBtn->setEnabled(false);

        QTimer::singleShot(7000, this, [=] {
            QProcess::startDetached("qdbus", { "--literal", "com.deepin.dde.shutdownFront", "/com/deepin/dde/shutdownFront", "com.deepin.dde.shutdownFront.Restart" });
            m_backupBtn->setEnabled(isEnable);
        });
    }

    self->deleteLater();
}
