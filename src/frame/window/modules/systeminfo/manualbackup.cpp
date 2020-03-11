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
    , m_tipsLabel(new QLabel(tr("Choose Directory invalid")))
    , m_backupBtn(new QPushButton(tr("Backup")))
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

    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    m_directoryChooseWidget->setFileDialog(fileDialog);

    connect(model, &BackupAndRestoreModel::backupButtonEnabledChanged, m_backupBtn, &QPushButton::setEnabled);
    connect(m_directoryChooseWidget, &DFileChooserEdit::dialogClosed, this, &ManualBackup::onChoose);
    connect(m_backupBtn, &QPushButton::clicked, this, &ManualBackup::backup, Qt::QueuedConnection);

    m_backupBtn->setEnabled(model->backupButtonEnabled());

    m_directoryChooseWidget->lineEdit()->setText(model->backupDirectory());
}

void ManualBackup::onChoose()
{
    m_backupBtn->setEnabled(false);

    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();
    if (choosePath.isEmpty()) {
        return;
    }

    if (!(choosePath.startsWith("/data") || choosePath.startsWith("/home"))) {
        m_tipsLabel->show();
        return;
    }

    m_tipsLabel->hide();

    m_model->setBackupButtonEnabled(!m_directoryChooseWidget->lineEdit()->text().isEmpty());
}

void ManualBackup::backup()
{
    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();

    if (!choosePath.isEmpty()) {
        Q_EMIT requestSetBackupDirectory(choosePath);
    }
}
