#include "systeminfo.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QLocale>

#include <fstream>

#include <libdui/dbaseline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/darrowlineexpand.h>

SystemInfo::SystemInfo()
    : m_dbusSystemInfo("com.deepin.daemon.SystemInfo", "/com/deepin/daemon/SystemInfo", QDBusConnection::sessionBus(), this)
{    
    DUI::DBaseLine *baseLine = new DUI::DBaseLine;
    baseLine->setText(tr("系统信息"));

    QLabel *deepinLogo = new QLabel;
    deepinLogo->setPixmap(QPixmap("modules/system_info/images/logo.png"));
    deepinLogo->setAlignment(Qt::AlignCenter);

    QLabel *deepinName = new QLabel(tr("Copyright (c) 2011-2014 Wuhan Deepin Technology Co., Ltd."));
    deepinName->setWordWrap(true);
    deepinName->setAlignment(Qt::AlignCenter);

    QLabel *info_sysVersion = new QLabel(tr("系统版本 :"));
    info_sysVersion->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_sysVersionContent = new QLabel(m_dbusSystemInfo.version());
    info_sysVersionContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    info_sysVersionContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLabel *info_sysType = new QLabel(tr("系统类型 :"));
    info_sysType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_sysTypeContent = new QLabel(QString(tr("%1位")).arg(m_dbusSystemInfo.systemType()));
    info_sysTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel *info_cpuType = new QLabel(tr("处理器 :"));
    info_cpuType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_cpuTypeContent = new QLabel(m_dbusSystemInfo.processor());
    info_cpuTypeContent->setWordWrap(true);
    info_cpuTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel *info_memory = new QLabel(tr("内存 :"));
    info_memory->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_memoryContent = new QLabel(formatCap(m_dbusSystemInfo.memoryCap()));
    info_memoryContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);


    QLabel *info_hardDrive = new QLabel(tr("硬盘 :"));
    info_hardDrive->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_hardDriveContent = new QLabel(formatCap(m_dbusSystemInfo.diskCap()));
    info_hardDriveContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QGridLayout *infoGrid = new QGridLayout;
    infoGrid->addWidget(info_sysVersion, 0, 0);
    infoGrid->addWidget(info_sysVersionContent, 0, 1);
    infoGrid->addWidget(info_sysType, 1, 0);
    infoGrid->addWidget(info_sysTypeContent, 1, 1);
    infoGrid->addWidget(info_cpuType, 2, 0);
    infoGrid->addWidget(info_cpuTypeContent, 2, 1);
    infoGrid->addWidget(info_memory, 3, 0);
    infoGrid->addWidget(info_memoryContent, 3, 1);
    infoGrid->addWidget(info_hardDrive, 4, 0);
    infoGrid->addWidget(info_hardDriveContent, 4, 1);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(deepinLogo);
    infoLayout->addWidget(deepinName);
    infoLayout->addLayout(infoGrid);
    infoLayout->setContentsMargins(10, 15, 10, 30);
    infoLayout->setSpacing(8);

    QWidget *infoWidget = new QWidget;
    infoWidget->setLayout(infoLayout);
    infoWidget->setStyleSheet("QLabel {color:#aaa; font-size:12px;} QWidget {background-color:#1a1b1b;}");

    QPlainTextEdit *licenseEdit = new QPlainTextEdit;
    licenseEdit->appendPlainText(getLicense("./modules/system_info/gpl/gpl-3.0-%1-%2.txt", "title") + "\n" +
                                 getLicense("./modules/system_info/gpl/gpl-3.0-%1-%2.txt", "body") + "\n");
    licenseEdit->setBackgroundVisible(false);
    licenseEdit->setFrameStyle(QFrame::NoFrame);
    licenseEdit->setStyleSheet("background-color:#1a1b1b; color:#666;");
    licenseEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    licenseEdit->verticalScrollBar()->hide();
    licenseEdit->setContextMenuPolicy(Qt::NoContextMenu);
    licenseEdit->setReadOnly(true);
    licenseEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    licenseEdit->setCursor(Qt::ArrowCursor);
    licenseEdit->moveCursor(QTextCursor::Start);

    DUI::DArrowLineExpand *license = new DUI::DArrowLineExpand;
    license->setTitle(tr("GNU 通用公共许可协议"));
    license->setContent(licenseEdit);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(baseLine);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(infoWidget);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(license);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_centeralFrame = new QFrame;
    m_centeralFrame->setLayout(centeralLayout);

    /*
    qDebug() << formatCap(qulonglong(1000));
    qDebug() << formatCap(qulonglong(1024));
    qDebug() << formatCap(qulonglong(1024) + 1);
    qDebug() << formatCap(qulonglong(1024) * 1024);
    qDebug() << formatCap(qulonglong(1024) * 1024);
    qDebug() << formatCap(qulonglong(1024) * 1024 + 1);
    qDebug() << formatCap(qulonglong(1024) * 1024 * 1024);
    qDebug() << formatCap(qulonglong(1024) * 1024 * 1024 + 1);
    qDebug() << formatCap(qulonglong(1024) * 1024 * 1024 * 1024);*/
}

SystemInfo::~SystemInfo()
{
    if (m_centeralFrame)
        m_centeralFrame->deleteLater();
}

QFrame *SystemInfo::getContent()
{
    return m_centeralFrame;
}

QString SystemInfo::formatCap(qulonglong cap) const
{
    QString type[] = {"B", "KB", "MB", "GB", "TB"};

    //qDebug() << cap;

    if (cap < qulonglong(1024))
        return QString::number(cap) + type[0];
    if (cap < qulonglong(1024) * 1024)
        return QString::number(double(cap) / 1024, 'f', 2) + type[1];
    if (cap < qulonglong(1024) * 1024 * 1024)
        return QString::number(double(cap) / 1024 / 1024, 'f', 2) + type[2];
    if (cap < qulonglong(1024) * 1024 * 1024 * 1024)
        return QString::number(double(cap) / 1024 / 1024 / 1024, 'f', 2) + type[3];

    return QString::number(double(cap) / 1024 / 1024 / 1024 / 1024, 'f', 2) + type[4];
}

QString SystemInfo::getLicense(const QString &filePath, const QString &type) const
{
    QString lang = QLocale::system().name();
    if (!lang.contains("zh"))
        lang = "en";
    QString path = QString(filePath).arg(lang).arg(type);

    qDebug() << path;

    std::ifstream license;
    license.open(path.toStdString(), std::ios::in);

    if (!license.is_open())
        return "";

    QByteArray buf;
    char c;
    while (license.get(c))
        buf.append(c);
    license.close();

    //qDebug() << buf;

    return std::move(buf);
}
