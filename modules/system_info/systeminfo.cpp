#include "systeminfo.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QLocale>
#include <QFile>

#include <libdui/dbaseline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/libdui_global.h>
#include <libdui/darrowlineexpand.h>

#include "moduleheader.h"

DUI_USE_NAMESPACE

SystemInfo::SystemInfo()
    : m_dbusSystemInfo("com.deepin.daemon.SystemInfo", "/com/deepin/daemon/SystemInfo", QDBusConnection::sessionBus(), this)
{
    QT_TRANSLATE_NOOP("ModuleName", "System Information");

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_baseLine = new ModuleHeader(tr("System Infomation"), false);

    QLabel *deepinLogo = new QLabel;
    deepinLogo->setPixmap(QPixmap(":/images/images/logo.png"));
    deepinLogo->setAlignment(Qt::AlignCenter);

    QLabel *deepinName = new QLabel(tr("Copyright (c) 2011-2015 Wuhan Deepin Technology Co., Ltd."));
    deepinName->setWordWrap(true);
    deepinName->setAlignment(Qt::AlignCenter);

    QLabel *info_sysVersion = new QLabel(tr("Deepin Edition:"));
    info_sysVersion->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_sysVersionContent = new QLabel(m_dbusSystemInfo.version());
    info_sysVersionContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    info_sysVersionContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLabel *info_sysType = new QLabel(tr("System Type:"));
    info_sysType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_sysTypeContent = new QLabel(QString(tr("%1 Bit")).arg(m_dbusSystemInfo.systemType()));
    info_sysTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel *info_cpuType = new QLabel(tr("Processor:"));
    info_cpuType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_cpuTypeContent = new QLabel(m_dbusSystemInfo.processor());
    info_cpuTypeContent->setWordWrap(true);
    info_cpuTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel *info_memory = new QLabel(tr("Memory:"));
    info_memory->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_memoryContent = new QLabel(formatCap(m_dbusSystemInfo.memoryCap()));
    info_memoryContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);


    QLabel *info_hardDrive = new QLabel(tr("Disk:"));
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

    m_infoWidget = new QWidget;
    m_infoWidget->setLayout(infoLayout);
    m_infoWidget->setStyleSheet("QLabel {color:#aaa; font-size:12px;} QWidget {background-color:#1a1b1b;}");

    m_licenseEdit = new QPlainTextEdit;
    m_licenseEdit->appendPlainText(getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "title") + "\n" +
                                 getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "body") + "\n");
    m_licenseEdit->setBackgroundVisible(false);
    m_licenseEdit->setFrameStyle(QFrame::NoFrame);
    m_licenseEdit->setStyleSheet("background-color:#1a1b1b; color:#666;");
    m_licenseEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_licenseEdit->verticalScrollBar()->hide();
    m_licenseEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_licenseEdit->setReadOnly(true);
    m_licenseEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    m_licenseEdit->setCursor(Qt::ArrowCursor);
    m_licenseEdit->moveCursor(QTextCursor::Start);
    m_licenseEdit->setFixedWidth(310);
    m_licenseEdit->installEventFilter(this);

    DArrowLineExpand *license = new DArrowLineExpand;
    license->setTitle(tr("GNU GENERAL PUBLIC LICENSE"));
    license->setContent(m_licenseEdit);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(m_baseLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_infoWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(license);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_centeralFrame = new QFrame;
    m_centeralFrame->setLayout(centeralLayout);
}

SystemInfo::~SystemInfo()
{
    m_centeralFrame->setParent(nullptr);

    /* TODO: 如果使用下面的 deleteLater 方法来析构，
        当这个插件被卸载的时候程序会崩溃，似乎是由于析构方法没有执行
        完而 .so 文件已经被 QPluginLoader 卸载
    */
    delete m_centeralFrame;
//    m_centeralFrame->deleteLater();
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
    if (lang != "zh_CN" && lang != "zh_TW")
        lang = "en";
    QString path = QString(filePath).arg(lang).arg(type);

    qDebug() << path;

    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return "";

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

void SystemInfo::updateLicenseWidget()
{
//    qDebug() << m_centeralFrame->height();
    m_licenseEdit->setFixedHeight(m_centeralFrame->height()
                                  - m_infoWidget->height()
                                  - m_baseLine->height()
                                  - 32 // 32 for DArrowLine
                                  - 6); // 6 for DSeparatorHorizontal * 3
}

bool SystemInfo::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_licenseEdit && e->type() == QEvent::Resize)
        updateLicenseWidget();

    return false;
}
