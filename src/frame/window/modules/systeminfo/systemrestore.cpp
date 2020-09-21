#include "systemrestore.h"

#include "manualbackup.h"
#include "manualrestore.h"

#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QTimer>
#include <QEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

DWIDGET_USE_NAMESPACE

SystemRestore::SystemRestore(BackupAndRestoreModel* model, QWidget *parent)
    : QWidget(parent)
//    , m_buttonBox(new DButtonBox(this)) 注释代码及新增改动为临时方案，解决QStacklayout问题后revert
    , m_backupPage(new ManualBackup(model))
    , m_restorePage(new ManualRestore(model))
{
    setObjectName("SystemRestore");

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    m_backupPage->setFileDialog(fileDialog);
    m_restorePage->setFileDialog(fileDialog);

//    DButtonBoxButton* backupBtn = new DButtonBoxButton(tr("Backup"));
//    DButtonBoxButton* restoreBtn = new DButtonBoxButton(tr("Restore"));

//    m_buttonBox->setButtonList({backupBtn, restoreBtn}, true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);





//    mainLayout->addWidget(m_buttonBox);
//    mainLayout->addSpacing(5);

//    m_stackedLayout = new QStackedLayout;
//    m_stackedLayout->addWidget(m_backupPage);
//    m_stackedLayout->addWidget(m_restorePage);

//    mainLayout->addLayout(m_stackedLayout);


    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(m_backupPage, tr("Backup"));
    tabWidget->addTab(m_restorePage, tr("Restore"));
    mainLayout->addWidget(tabWidget);


    setLayout(mainLayout);

//    connect(m_buttonBox, &DButtonBox::buttonClicked, this, [=] (QAbstractButton *button) {
//        const QMap<QAbstractButton*, QWidget*> page {
//            {backupBtn, m_backupPage},
//            {restoreBtn, m_restorePage},
//        };

//        m_backupPage->setTipsVisible(false);
//        m_restorePage->setTipsVisible(false);
//        m_stackedLayout->setCurrentWidget(page[button]);
//    });

//    backupBtn->setChecked(true);

    connect(m_backupPage, &ManualBackup::requestSetManualBackupDirectory, this, &SystemRestore::requestSetManualBackupDirectory);
    connect(m_backupPage, &ManualBackup::requestSetSystemBackupDirectory, this, &SystemRestore::requestSetSystemBackupDirectory);
    connect(m_restorePage, &ManualRestore::requestManualRestore, this, &SystemRestore::requestManualRestore);
    connect(m_restorePage, &ManualRestore::requestSystemRestore, this, &SystemRestore::requestSystemRestore);
}
