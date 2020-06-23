#include "manualbackup.h"
#include "backupandrestoremodel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <DBackgroundGroup>
#include <QPushButton>
#include <QDir>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

ManualBackup::ManualBackup(BackupAndRestoreModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_directoryChooseWidget(new DFileChooserEdit)
    , m_tipsLabel(new QLabel)
    , m_backupBtn(new QPushButton(tr("Backup")))
    , m_loadingIndicator(new DWaterProgress)
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
    mainLayout->addWidget(m_loadingIndicator, 0, Qt::AlignHCenter);

    setLayout(mainLayout);

    m_tipsLabel->hide();

    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    m_directoryChooseWidget->setFileDialog(fileDialog);

    connect(model, &BackupAndRestoreModel::backupButtonEnabledChanged, this, [=](bool enable) {
        m_backupBtn->setVisible(enable);
        m_loadingIndicator->setVisible(!enable);
    });
    connect(m_directoryChooseWidget, &DFileChooserEdit::dialogClosed, this, &ManualBackup::onChoose);
    connect(m_backupBtn, &QPushButton::clicked, this, &ManualBackup::backup, Qt::QueuedConnection);
    connect(m_directoryChooseWidget->lineEdit(), &QLineEdit::textChanged, this, [=](const QString& text) {
        m_backupBtn->setEnabled(model->backupButtonEnabled() && !text.isEmpty());
    });
    connect(model, &BackupAndRestoreModel::manualBackupErrorTypeChanged, this, &ManualBackup::onManualBackupErrorTypeChanged);

    m_backupBtn->setEnabled(!model->backupDirectory().isEmpty());
    m_backupBtn->setVisible(model->backupButtonEnabled());
    m_loadingIndicator->setVisible(!model->backupButtonEnabled());

    m_directoryChooseWidget->lineEdit()->setText(model->backupDirectory());

    m_loadingIndicator->setValue(50);
    m_loadingIndicator->setTextVisible(false);
    m_loadingIndicator->setFixedSize(48, 48);
    m_loadingIndicator->start();

    onManualBackupErrorTypeChanged(model->manualBackupErrorType());
}

void ManualBackup::onChoose()
{
    m_backupBtn->setEnabled(false);
    m_tipsLabel->hide();

    // TODO(justforlxz): need send signal to worker -> model;
    m_model->setBackupButtonEnabled(!m_directoryChooseWidget->lineEdit()->text().isEmpty());
    m_backupBtn->setEnabled(true);
}

void ManualBackup::backup()
{
    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();

    if (!choosePath.isEmpty()) {
        m_backupBtn->setVisible(false);
        m_loadingIndicator->setVisible(true);
        Q_EMIT requestSetBackupDirectory(choosePath);
    }
}

void ManualBackup::onManualBackupErrorTypeChanged(ErrorType type)
{
    m_tipsLabel->setVisible(true);

    switch (type) {
    case ErrorType::PathError: {
        m_tipsLabel->setText(tr("Invalid path"));
        break;
    }
    case ErrorType::SpaceError: {
        m_tipsLabel->setText(tr("Insufficient disk space"));
        break;
    }
    case ErrorType::FsError: {
        m_tipsLabel->setText(tr("Only storage devices in EXT4, NTFS format are supported"));
        break;
    }
    case ErrorType::ToolError: {
        m_tipsLabel->setText(tr("Tool execution error"));
        break;
    }
    default: {
        m_tipsLabel->setVisible(false);
        break;
    }
    }
}
