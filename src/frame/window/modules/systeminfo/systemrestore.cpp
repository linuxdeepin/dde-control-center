#include "systemrestore.h"

#include "manualbackup.h"
#include "manualrestore.h"

#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

DWIDGET_USE_NAMESPACE

SystemRestore::SystemRestore(QWidget *parent)
    : QWidget(parent)
    , m_buttonBox(new DButtonBox(this))
    , m_backupPage(new ManualBackup)
    , m_restorePage(new ManualRestore)
{
    DButtonBoxButton* backupBtn = new DButtonBoxButton(tr("Backup"));
    DButtonBoxButton* restoreBtn = new DButtonBoxButton(tr("Restore"));

    m_buttonBox->setButtonList({backupBtn, restoreBtn}, true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_buttonBox);
    mainLayout->addSpacing(5);

    QStackedLayout* stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(m_backupPage);
    stackedLayout->addWidget(m_restorePage);

    mainLayout->addLayout(stackedLayout);

    setLayout(mainLayout);

    connect(m_buttonBox, &DButtonBox::buttonClicked, this, [=] (QAbstractButton *button) {
        const QMap<QAbstractButton*, QWidget*> page {
            {backupBtn, m_backupPage},
            {restoreBtn, m_restorePage},
        };

        stackedLayout->setCurrentWidget(page[button]);
    });

    backupBtn->setChecked(true);
}
