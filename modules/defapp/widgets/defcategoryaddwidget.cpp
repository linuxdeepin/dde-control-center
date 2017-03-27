#include "defcategoryaddwidget.h"
#include "funcbutton.h"
#include <QObject>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>

using namespace dcc;
using namespace dcc::defapp;
DefCategoryAddWidget::DefCategoryAddWidget(QWidget *parent)
    : SettingsItem(parent)
{
    setFixedHeight(36);
    m_add = new FuncButton;
    m_add->setAlignment(Qt::AlignCenter);
    m_add->setText(tr("Add"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(10, 0, 0, 0);
    mainLayout->addSpacing(31);
    mainLayout->addWidget(m_add);
    mainLayout->addStretch();

    setLayout(mainLayout);
    connect(m_add, &FuncButton::clicked, this, &DefCategoryAddWidget::clicked);
    setObjectName("DefCategoryAddWidget");
}

void DefCategoryAddWidget::clicked()
{
    do {
        if (!isEnabled())
            break;

        emit requestFrameAutoHide(false);
        QFileDialog dialog;
        dialog.setWindowTitle(tr("Open Desktop file"));

        QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        if (!directory.isEmpty())
            dialog.setDirectory(directory.first());

        if (dialog.exec() != QDialog::Accepted)
            break;

        QString path = dialog.selectedFiles()[0];

        if (path.isEmpty())
            break;

        QFileInfo info(path);
        emit requestCreateFile(m_category, info);
    } while(false);

    QTimer::singleShot(500, this, [ = ] { emit requestFrameAutoHide(true); });
}


