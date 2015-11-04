#include "systeminfo.h"
#include "moduleheader.h"
#include "constants.h"
#include "mirrorscontrolwidget.h"
#include "updatewidget.h"
#include "applictionitemwidget.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QScrollBar>
#include <QLocale>
#include <QFile>
#include <QScrollArea>

#include <libdui/dbaseline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/libdui_global.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dboxwidget.h>

DUI_USE_NAMESPACE

SystemInfo::SystemInfo()
    : m_dbusSystemInfo("com.deepin.daemon.SystemInfo", "/com/deepin/daemon/SystemInfo", QDBusConnection::sessionBus(), this)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "System Information"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    // to use gtk functions
    gtk_init(nullptr, nullptr);

    m_baseLine = new ModuleHeader(tr("System Information"), false);

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
    QLabel *info_memoryContent = new QLabel(UpdateWidget::formatCap(m_dbusSystemInfo.memoryCap()));
    info_memoryContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel *info_hardDrive = new QLabel(tr("Disk:"));
    info_hardDrive->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel *info_hardDriveContent = new QLabel(UpdateWidget::formatCap(m_dbusSystemInfo.diskCap()));
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

    QLabel *licenseTitle = new QLabel;
    licenseTitle->setText(getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "title"));
    licenseTitle->setStyleSheet("color:#666;");
    licenseTitle->setAlignment(Qt::AlignCenter);
    licenseTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    licenseTitle->setFixedWidth(DCC::ModuleContentWidth);

    QLabel *licenseBody = new QLabel;
    licenseBody->setText(getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "body"));
    licenseBody->setStyleSheet("color:#666;");
    licenseBody->setAlignment(Qt::AlignTop);
    licenseBody->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    licenseBody->setFixedWidth(DCC::ModuleContentWidth);
    licenseBody->setWordWrap(true);

    DVBoxWidget *licenseWidget = new DVBoxWidget;
    licenseWidget->layout()->addWidget(licenseTitle);
    licenseWidget->layout()->addWidget(licenseBody);
    licenseWidget->layout()->setContentsMargins(2, 5, 2, 2);
    licenseWidget->setFixedWidth(DCC::ModuleContentWidth);

    m_licenseArea = new QScrollArea;
    m_licenseArea->setFrameStyle(QFrame::NoFrame);
    m_licenseArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_licenseArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_licenseArea->setWidget(licenseWidget);
    m_licenseArea->setFixedWidth(DCC::ModuleContentWidth);
    m_licenseArea->setStyleSheet("background-color:#1a1b1b;");

    DArrowLineExpand *license = new DArrowLineExpand;
    license->setTitle(tr("GNU GENERAL PUBLIC LICENSE"));
    license->setContent(m_licenseArea);

    m_updateInfoWidget = new UpdateWidget;
    m_mirrorsControlWidget = new MirrorsControlWidget;
    m_mirrorsControlWidget->hide();

    QVBoxLayout *tLayout = new QVBoxLayout;
    tLayout->addWidget(m_updateInfoWidget);
    tLayout->addWidget(m_mirrorsControlWidget);
    tLayout->setMargin(0);
    tLayout->setSpacing(0);
    m_updateWidget = new QWidget;
    m_updateWidget->setLayout(tLayout);

    m_updateExpand = new UpdateArrowExpand;
    m_updateExpand->setContent(m_updateWidget);
    m_updateExpand->setUpdatableNums(0); // 0 for default

    DExpandGroup *expandGrp = new DExpandGroup(this);
    expandGrp->addExpand(license);
    expandGrp->addExpand(m_updateExpand);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(m_baseLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_infoWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(license);
    centeralLayout->addWidget(m_updateExpand);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_centeralFrame = new QFrame;
    m_centeralFrame->installEventFilter(this);
    m_centeralFrame->setLayout(centeralLayout);

    connect(m_updateInfoWidget, &UpdateWidget::updatableNumsChanged, this, &SystemInfo::onUpdatableNumsChange);
    connect(m_updateExpand, &UpdateArrowExpand::configButtonClicked, [this] {
        m_updateInfoWidget->hide();
        m_mirrorsControlWidget->show();
    });
    connect(m_mirrorsControlWidget, &MirrorsControlWidget::configAccept, [this] {
        m_updateInfoWidget->show();
        m_mirrorsControlWidget->hide();
    });
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

void SystemInfo::updateWidgetHeight()
{
//    qDebug() << m_centeralFrame->height();
    const int expandContentHeight = m_centeralFrame->height()
                                    - m_infoWidget->height()
                                    - m_baseLine->height()
                                    - 32 * 2 // 32 for DArrowLine
                                    - 2 * 3; // 2 for DSeparatorHorizontal
    m_licenseArea->setFixedHeight(expandContentHeight);
    m_updateWidget->setFixedHeight(expandContentHeight);
    m_updateInfoWidget->setFixedHeight(expandContentHeight);
    m_mirrorsControlWidget->setFixedHeight(expandContentHeight);
    m_updateExpand->updateContentHeight();
}

bool SystemInfo::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_centeralFrame && e->type() == QEvent::Resize)
        updateWidgetHeight();

    return false;
}

void SystemInfo::onUpdatableNumsChange(const int updatableNums)
{
    m_updateExpand->setUpdatableNums(updatableNums);

    if (updatableNums && !m_updateExpand->expand())
        m_updateExpand->setExpand(true);
}
