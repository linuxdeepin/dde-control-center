#include"restoreclient.h"
#include <QVBoxLayout>
#include "widgets/translucentframe.h"
#include "widgets/settingsitem.h"
#include <ddialog.h>
#include <qprocess.h>
#include <QApplication>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::systeminfo;

DWIDGET_USE_NAMESPACE


CRestoreClientPage::CRestoreClientPage(QWidget *parent):dcc::ContentWidget(parent)
{
    initUI();
}

void CRestoreClientPage::initUI()
{

    QVBoxLayout *layout = new  QVBoxLayout;

    auto container = new dcc::widgets::TranslucentFrame(this);

    QPushButton *btn = new QPushButton(tr("Reset to factory settings"), container);

    layout->addWidget(btn);
    layout->addStretch();
    container->setLayout(layout);
    setContent(container);

    connect(btn, &QPushButton::clicked, this, &CRestoreClientPage::restore);
}

void CRestoreClientPage::restore()
{

        DDialog dialog;
        QString message{ tr("It will reset system settings to their defaults without affecting your files and apps, but the username and password will be cleared, please confirm before proceeding") };


        dialog.setMessage(message);
        dialog.addButton(tr("Cancel"));

        int result = dialog.addButton(tr("Confirm"), true, DDialog::ButtonWarning);
        if (dialog.exec() != result) {
            return;
        }
        QProcess *process = new QProcess;
        process->start("pkexec",QStringList()<<RESTORE_TOOL_PATH<<"--reset");
}
