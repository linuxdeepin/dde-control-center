#include "manualbackup.h"
#include "backupandrestoremodel.h"

#include <DFontSizeManager>
#include <DListView>
#include <DStandardItem>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QLabel>
#include <DBackgroundGroup>
#include <QPushButton>
#include <QStandardItemModel>
#include <QDir>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;
DWIDGET_USE_NAMESPACE

ManualBackup::ManualBackup(BackupAndRestoreModel* model, QWidget* parent)
    : QWidget(parent)
    , m_actionType(ActionType::ManualBackup)
    , m_model(model)
    , m_directoryChooseWidget(new DFileChooserEdit)
    , m_tipsLabel(new DTipLabel)
    , m_backupBtn(new QPushButton(tr("Backup")))
    , m_loadingIndicator(new DWaterProgress)
{
    m_tipsLabel->setWordWrap(true);
    auto pa = DApplicationHelper::instance()->palette(m_tipsLabel);
    pa.setBrush(DPalette::TextTips, Qt::red);
    DApplicationHelper::instance()->setPalette(m_tipsLabel, pa);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);

    QLabel *backupType = new QLabel(tr("Backup Type"));
    QLabel *savePath = new QLabel(tr("Save to"));
    DFontSizeManager::instance()->bind(backupType, DFontSizeManager::T5);
    DFontSizeManager::instance()->bind(savePath, DFontSizeManager::T5);
    DListView *backupTypeView = new DListView;
    backupTypeView->setMaximumHeight(80);
    backupTypeView->setItemSpacing(5);
    backupTypeView->setFrameShape(QFrame::NoFrame);
    backupTypeView->setViewportMargins({});
    backupTypeView->setMovement(QListView::Static);
    backupTypeView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    backupTypeView->setAutoScroll(false);
    backupTypeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    backupTypeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    backupTypeView->setSelectionMode(QAbstractItemView::NoSelection);

    QStandardItemModel *itemModel = new QStandardItemModel;
    DStandardItem *manualBackupItem = new DStandardItem;
    DStandardItem *systemBackupItem = new DStandardItem;
    manualBackupItem->setCheckState(Qt::Checked);
    manualBackupItem->setText(tr("Full Backup"));
    systemBackupItem->setText(tr("System Backup"));
    itemModel->appendRow(manualBackupItem);
    itemModel->appendRow(systemBackupItem);
    backupTypeView->setModel(itemModel);

    mainLayout->addWidget(backupType, 0);
    mainLayout->addWidget(backupTypeView, 0, Qt::AlignVCenter);
    mainLayout->addWidget(savePath, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_directoryChooseWidget);

    mainLayout->addWidget(m_tipsLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(m_backupBtn);
    mainLayout->addWidget(m_loadingIndicator, 0, Qt::AlignHCenter);

    setLayout(mainLayout);

    m_tipsLabel->hide();

    m_directoryChooseWidget->setFileMode(QFileDialog::Directory);

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

    connect(backupTypeView, &DListView::clicked, this, [ = ](const QModelIndex &index){
        for (int i = 0; i < itemModel->rowCount(); i++) {
            itemModel->item(i, 0)->setCheckState(Qt::Unchecked);
        }
        itemModel->item(index.row(), 0)->setCheckState(Qt::Checked);
        m_actionType = index.row() > 0 ? ActionType::SystemBackup : ActionType::ManualBackup;
    });

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

void ManualBackup::setTipsVisible(const bool &visible)
{
    m_tipsLabel->setVisible(visible);
}

void ManualBackup::onChoose()
{
    m_tipsLabel->hide();

    // TODO(justforlxz): need send signal to worker -> model;
    m_backupBtn->setEnabled(!m_directoryChooseWidget->lineEdit()->text().isEmpty());
}

void ManualBackup::backup()
{
    const QString& choosePath = m_directoryChooseWidget->lineEdit()->text();

    if (!choosePath.isEmpty()) {
        m_backupBtn->setVisible(false);
        m_loadingIndicator->setVisible(true);
        if (m_actionType == ActionType::ManualBackup)
            Q_EMIT requestSetManualBackupDirectory(choosePath);
        else
            Q_EMIT requestSetSystemBackupDirectory(choosePath);
    }
}

void ManualBackup::onManualBackupErrorTypeChanged(ErrorType type)
{
    m_tipsLabel->setVisible(true);

    switch (type) {
    case ErrorType::PathError: {
        m_tipsLabel->setText(tr("The storage location cannot be in source disk, please reselect"));
        break;
    }
    case ErrorType::PathError2: {
        m_tipsLabel->setText(tr("Invalid path"));
        break;
    }
    case ErrorType::GrubError: {
        m_tipsLabel->setText(tr("Grub authentication failed"));
        break;
    }
    default: {
        m_tipsLabel->setVisible(false);
        break;
    }
    }
}
