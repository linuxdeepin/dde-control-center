#include "defcategoryaddwidget.h"
#include "funcbutton.h"
#include <QObject>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QMessageBox>
#include <sys/types.h>    // defines special types
#include <pwd.h>    // defines the passwd structure
#include <unistd.h>//header for getuid system call
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

bool DefCategoryAddWidget::createDesktopFile(const QFileInfo &info)
{
    struct passwd *user;
    user = getpwuid(getuid());
    //create desktop file in ~/.local/share/applications/
    QFile file(QString(user->pw_dir) + "/.local/share/applications/deepin-custom-" + info.baseName() + ".desktop");
    //方式：Append为追加，WriteOnly，ReadOnly
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    qDebug() << file.fileName();
    QTextStream out(&file);
    out << "[Desktop Entry]\n"
        "Type=Application\n"
        "Version=1.0\n"
        "Name=" + info.baseName() + "\n"
        "Path=" + info.path() + "\n"
        "Exec=" +  info.filePath() + "\n"
        "Icon=application-default-icon\n"
        "Terminal=false\n"
        "Categories=" + m_category + ";"
        << endl;
    out.flush();
    file.close();
    return true;
}

void DefCategoryAddWidget::clicked()
{
    do {
        if (!isEnabled())
            break;

        emit requestFrameAutoHide(false);
        QFileDialog dialog;
        dialog.setWindowTitle(tr("Open Desktop file"));
        dialog.setNameFilter(tr("Desktop Files(*.desktop);; All Files(*)"));

        if (dialog.exec() != QDialog::Accepted)
            break;

        QString path = dialog.selectedFiles()[0];

        if (path.isEmpty())
            break;

        QFileInfo info(path);

        if (!info.exists() || !info.isExecutable())
            break;

        if (info.suffix() == "desktop") {
            emit addUserItem(m_category, info.filePath());
        } else if (createDesktopFile(info)) {
            emit addUserItem(m_category, info.filePath());
        }
    } while(false);

    QTimer::singleShot(500, this, [ = ] { emit requestFrameAutoHide(true); });
}


