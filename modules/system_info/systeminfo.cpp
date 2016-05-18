/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "systeminfo.h"
#include "moduleheader.h"
#include "constants.h"
#include "mirrorscontrolwidget.h"
#include "updatewidget.h"
#include "applictionitemwidget.h"
#include "licensescanner.h"
#include "mousearea.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QScrollBar>
#include <QLocale>
#include <QFile>
#include <QScrollArea>

#include <dbaseline.h>
#include <dseparatorhorizontal.h>
#include <dwidget_global.h>
#include <darrowlineexpand.h>
#include <dboxwidget.h>

DWIDGET_USE_NAMESPACE

QFrame *SystemInfoModule::getContent()
{
    qDebug() << "Begin new SystemInfo";
    if (NULL == m_systeminfo) {
        m_systeminfo = new SystemInfo(this);
    }
    qDebug() << "End new SystemInfo";
    return m_systeminfo->getContent();
}

void SystemInfoModule::preUnload()
{
    if (m_systeminfo) {
        return m_systeminfo->preUnload();
    }
}

SystemInfo::SystemInfo(QObject *parent)
    : QObject(parent),
      m_dbusSystemInfo("com.deepin.daemon.SystemInfo",
                       "/com/deepin/daemon/SystemInfo",
                       QDBusConnection::sessionBus(),
                       this)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "System Information"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_centeralFrame = new QFrame;
    DExpandGroup *expandGrp = new DExpandGroup(this);
    m_licenseArea = new QScrollArea;
    DArrowLineExpand *license = new DArrowLineExpand;
    DVBoxWidget *licenseWidget = new DVBoxWidget;
    QLabel *licenseBody = new QLabel;
    QLabel *licenseTitle = new QLabel;
    QLabel *info_hardDriveContent = new QLabel(UpdateWidget::formatCap(m_dbusSystemInfo.diskCap()));
    QLabel *info_hardDrive = new QLabel(tr("Disk:"));
    QLabel *info_memoryContent = new QLabel(UpdateWidget::formatCap(m_dbusSystemInfo.memoryCap()));
    QLabel *info_memory = new QLabel(tr("Memory:"));
    QLabel *info_cpuTypeContent = new QLabel(m_dbusSystemInfo.processor());
    QLabel *info_cpuType = new QLabel(tr("Processor:"));
    QLabel *info_sysTypeContent = new QLabel(QString(tr("%1 Bit")).arg(m_dbusSystemInfo.systemType()));
    QLabel *info_sysType = new QLabel(tr("System Type:"));
    QLabel *info_sysDistroContent = new QLabel(QString("%1 %2").arg(m_dbusSystemInfo.distroDesc(), m_dbusSystemInfo.distroVer()));
    QLabel *info_sysVersionContent = new QLabel(m_dbusSystemInfo.version());
    QLabel *info_sysDistro = new QLabel(tr("Distribution:"));
    QLabel *info_sysVersion = new QLabel(tr("Deepin Edition:"));
    QLabel *deepinName = new QLabel(tr("Copyright (c) 2011-2016 Wuhan Deepin Technology Co., Ltd."));
    QLabel *deepinLogo = new QLabel;
    QGridLayout *infoGrid = new QGridLayout;
    QVBoxLayout *infoLayout = new QVBoxLayout;
    m_infoWidget = new QWidget;
    m_baseLine = new ModuleHeader(tr("System Information"), false);
    QVBoxLayout *centeralLayout = new QVBoxLayout;

    m_centeralFrame->installEventFilter(this);
    m_centeralFrame->setLayout(centeralLayout);

    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);
    centeralLayout->addWidget(m_baseLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_infoWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(license);

    m_infoWidget->setLayout(infoLayout);
    m_infoWidget->setStyleSheet("QLabel {color:#aaa; font-size:12px;} QWidget {background-color:#1a1b1b;}");

    infoLayout->addWidget(deepinLogo);
    infoLayout->addWidget(deepinName);
    infoLayout->addLayout(infoGrid);
    infoLayout->setContentsMargins(10, 15, 10, 30);
    infoLayout->setSpacing(8);

    infoGrid->addWidget(info_sysVersion, 0, 0);
    infoGrid->addWidget(info_sysVersionContent, 0, 1);

    const QString distroId = m_dbusSystemInfo.distroId();
    if (!distroId.isEmpty() && distroId.compare("Deepin", Qt::CaseInsensitive)) {
        infoGrid->addWidget(info_sysDistro, 1, 0);
        infoGrid->addWidget(info_sysDistroContent, 1, 1);
    } else {
        info_sysDistro->deleteLater();
        info_sysDistroContent->deleteLater();
    }

    infoGrid->addWidget(info_sysType, 2, 0);
    infoGrid->addWidget(info_sysTypeContent, 2, 1);
    infoGrid->addWidget(info_cpuType, 3, 0);
    infoGrid->addWidget(info_cpuTypeContent, 3, 1);
    infoGrid->addWidget(info_memory, 4, 0);
    infoGrid->addWidget(info_memoryContent, 4, 1);
    infoGrid->addWidget(info_hardDrive, 5, 0);
    infoGrid->addWidget(info_hardDriveContent, 5, 1);

    loadSystemInfoFromLocalFile(infoGrid);

    deepinLogo->setPixmap(QPixmap(":/images/images/logo.png"));
    deepinLogo->setAlignment(Qt::AlignCenter);

    deepinName->setWordWrap(true);
    deepinName->setAlignment(Qt::AlignCenter);
    deepinName->setStyleSheet("padding:2px 0;");

    info_sysVersion->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_sysVersionContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    info_sysVersionContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    info_sysDistro->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_sysDistroContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    info_sysDistroContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    info_sysType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_sysTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    info_cpuType->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_cpuTypeContent->setWordWrap(true);
    info_cpuTypeContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    info_memory->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_memoryContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    info_hardDrive->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    info_hardDriveContent->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    expandGrp->addExpand(license);

    license->setTitle(tr("GNU GENERAL PUBLIC LICENSE"));
    license->setContent(m_licenseArea);

    m_licenseArea->setFrameStyle(QFrame::NoFrame);
    m_licenseArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_licenseArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_licenseArea->setWidget(licenseWidget);
    m_licenseArea->setFixedWidth(DCC::ModuleContentWidth);
    m_licenseArea->setStyleSheet("background-color:#1a1b1b;");

    licenseWidget->layout()->addWidget(licenseTitle);
    licenseWidget->layout()->addWidget(licenseBody);

    licenseTitle->setText(getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "title"));
    licenseTitle->setStyleSheet("color:#666;");
    licenseTitle->setAlignment(Qt::AlignCenter);
    licenseTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    licenseTitle->setFixedWidth(DCC::ModuleContentWidth);

    licenseBody->setText(getLicense(":/licenses/gpl/gpl-3.0-%1-%2.txt", "body"));
    licenseBody->setStyleSheet("color:#666;");
    licenseBody->setAlignment(Qt::AlignTop);
    licenseBody->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    licenseBody->setFixedWidth(DCC::ModuleContentWidth);
    licenseBody->setWordWrap(true);
    licenseBody->setMargin(10);

    licenseWidget->layout()->setContentsMargins(2, 5, 2, 2);
    licenseWidget->setFixedWidth(DCC::ModuleContentWidth);

#ifdef DCC_SYSINFO_UPDATE
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
    centeralLayout->addWidget(m_updateExpand);

    expandGrp->addExpand(m_updateExpand);
#endif // DCC_SYSINFO_UPDATE

    scanlicenses();
    foreach(DArrowLineExpand * expand, m_extralicenses) {
        expandGrp->addExpand(expand);
        centeralLayout->addWidget(expand);
    }

#ifdef DCC_SYSINFO_UPDATE
    centeralLayout->addWidget(m_updateExpand);
#endif
    centeralLayout->addStretch(1);

#ifdef DCC_SYSINFO_UPDATE
    connect(m_updateInfoWidget, &UpdateWidget::updatableNumsChanged, this, &SystemInfo::onUpdatableNumsChange);
    connect(m_updateExpand, &UpdateArrowExpand::configButtonClicked, [this] {
        m_updateInfoWidget->hide();
        m_mirrorsControlWidget->show();
    });
    connect(m_mirrorsControlWidget, &MirrorsControlWidget::configAccept, [this] {
        m_updateInfoWidget->show();
        m_mirrorsControlWidget->hide();
    });
#endif
}

SystemInfo::~SystemInfo()
{
    m_centeralFrame->hide();
    m_centeralFrame->setParent(nullptr);
    m_centeralFrame->deleteLater();
}

QFrame *SystemInfo::getContent()
{
    return m_centeralFrame;
}

void SystemInfo::preUnload()
{
    // TODO/FIXME: 由于 QScrollArea 的绘图Bug，在动画切换的时候会穿透到桌面，所以这里先隐藏
#if DCC_SYSINFO_UPDATE && !defined(ARCH_MIPSEL) // modules are cached on mipsel, so don't hide widget.
    m_updateExpand->setVisible(false);
#endif
}

QString SystemInfo::getLicense(const QString &filePath, const QString &type) const
{
    QString lang = QLocale::system().name();
    if (lang != "zh_CN" && lang != "zh_TW") {
        lang = "en";
    }
    QString path = QString(filePath).arg(lang).arg(type);

    qDebug() << path;

    QFile license(path);
    if (!license.open(QIODevice::ReadOnly)) {
        return "";
    }

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
#ifdef DCC_SYSINFO_UPDATE
                                    - 32 * 2 // 32 for DArrowLine
#else
                                    - 32 * 1 // 32 for DArrowLine
#endif
                                    - 32 * m_extralicenses.length()
                                    - 2 * 3; // 2 for DSeparatorHorizontal
    m_licenseArea->setFixedHeight(expandContentHeight);

#ifdef DCC_SYSINFO_UPDATE
    m_updateWidget->setFixedHeight(expandContentHeight);
    m_updateInfoWidget->setFixedHeight(expandContentHeight);
    m_mirrorsControlWidget->setFixedHeight(expandContentHeight);
#endif
}

bool SystemInfo::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_centeralFrame && e->type() == QEvent::Resize) {
        updateWidgetHeight();
    }

    return false;
}

void SystemInfo::onUpdatableNumsChange(const int apps, const int packages)
{
    const int num = apps ? apps : !!packages;

    m_updateExpand->setUpdatableNums(num);

    if (num && !m_updateExpand->expand()) {
        m_updateExpand->setExpand(true);
    }
}

void SystemInfo::onProcessFinished()
{
    QProcess *process = qobject_cast<QProcess *>(sender());

    if (process) {
        process->terminate();
        process->kill();
        process->deleteLater();
        m_markActionStarted.remove(process->objectName());
    }
}

void SystemInfo::scanlicenses()
{
    QMap<QString, QString> licenseInfos = licenseScanner::scan();

    foreach(QString title, licenseInfos.keys()) {
        QLabel *_content = new QLabel(licenseInfos[title]);
        _content->setStyleSheet("color:#666;");
        _content->setAlignment(Qt::AlignTop);
        _content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _content->setFixedWidth(DCC::ModuleContentWidth);
        _content->setWordWrap(true);
        _content->setMargin(10);

        QScrollArea *content = new QScrollArea;
        content->setFrameStyle(QFrame::NoFrame);
        content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        content->setWidget(_content);
        content->setFixedWidth(DCC::ModuleContentWidth);
        content->setStyleSheet("background-color:#1a1b1b;");

        DArrowLineExpand *license = new DArrowLineExpand;
        license->setTitle(title);
        license->setContent(content);

        m_extralicenses.append(license);
    }
}

void SystemInfo::loadSystemInfoFromLocalFile(QGridLayout *infoGrid)
{
    QStringList info_dirs = QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);
    QSet<QString> markRead;

    foreach(const QString & path, info_dirs) {
        if (path.startsWith("/home") || path.startsWith("/root")) {
            continue;
        }

        QDir dir(path + "/systeminfos");

        dir.setNameFilters(QStringList() << "*.json");
        dir.setFilter(QDir::Files | QDir::Readable);

        if (dir.isReadable()) {
            foreach(const QFileInfo & file_info, dir.entryInfoList()) {
                if (markRead.contains(file_info.fileName())) {
                    continue;
                }

                markRead << file_info.fileName();

                QFile file(file_info.absoluteFilePath());

                if (file.open(QIODevice::ReadOnly)) {
                    const QJsonDocument &json_doc = QJsonDocument::fromJson(file.readAll());

                    for (const QJsonValue &value : json_doc.array()) {
                        const QJsonObject &json_obj = value.toObject();
                        int infoGridCount = infoGrid->rowCount();

                        QString title = json_obj.value("title").toString();
                        QString content = json_obj.value("content").toString();

                        if (!title.isEmpty()) {
                            QLabel *info_title = new QLabel;

                            info_title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
                            info_title->setTextFormat(Qt::AutoText);
                            info_title->setText(title + (content.isEmpty() ? "" : ":"));

                            infoGrid->addWidget(info_title, infoGridCount, 0);
                        } else if (content.isEmpty()) {
                            continue;
                        }

                        if (!content.isEmpty()) {
                            QLabel *info_content = new QLabel(content);

                            info_content->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                            info_content->setTextFormat(Qt::AutoText);
                            info_content->setText(content);

                            infoGrid->addWidget(info_content, infoGridCount, 1);

                            const QString &action = json_obj.value("action").toString();

                            if (!action.isEmpty()) {
                                MouseArea *mouse_area = new MouseArea(info_content);

                                mouse_area->resize(info_content->sizeHint());

                                connect(mouse_area, &MouseArea::clicked, [this, action] {
                                    if (m_markActionStarted.contains(action))
                                        return;

                                    m_markActionStarted << action;

                                    QProcess *process = new QProcess(this);

                                    process->setObjectName(action);

                                    connect(process, SIGNAL(error(QProcess::ProcessError)),
                                    SLOT(onProcessFinished()));
                                    connect(process, SIGNAL(finished(int)), SLOT(onProcessFinished()));
                                    process->start(action, QProcess::ReadOnly);
                                });
                            }
                        }
                    }
                }
            }
        }
    }
}
