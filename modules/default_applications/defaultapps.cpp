#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QObject>

#include "defaultapps.h"

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dswitchbutton.h>

DefaultApps::DefaultApps()
{
    Q_INIT_RESOURCE(dark);

    m_centralWidget = new QFrame;

    QSpacerItem *hSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *vSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QLabel *lbl_title = new QLabel(tr("Default Applications"));
    lbl_title->setStyleSheet("font-size:16px; font-weight:bold; color:#fff;");
    DUI::DTextButton *btn_reset = new DUI::DTextButton(tr("Reset"));

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(lbl_title);
    titleLayout->addItem(hSpacer);
    titleLayout->addWidget(btn_reset);
    titleLayout->setContentsMargins(15, 15, 15, 15);
    titleLayout->setSpacing(0);

    DUI::DBaseLine *defaultApplications = new DUI::DBaseLine;
    QLabel *lbl_defaultApps = new QLabel(tr("Default Applications"));
    lbl_defaultApps->setStyleSheet("font-size:14px; font-weight:bold; color:#ccc;");
    defaultApplications->setLeftContent(lbl_defaultApps);
    DUI::DBaseLine *autoPlayApplications = new DUI::DBaseLine;
    QLabel *lbl_autoPlay = new QLabel(tr("AutoPlay"));
    lbl_autoPlay->setStyleSheet("font-size:14px; font-weight:bold; color:#ccc;");
    autoPlayApplications->setLeftContent(lbl_autoPlay);
    DUI::DSwitchButton *switchBtn = new DUI::DSwitchButton;
    switchBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    autoPlayApplications->setRightContent(switchBtn);

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->addWidget(defaultApplications);
    scrollLayout->addWidget(new DUI::DSeparatorHorizontal);
    scrollLayout->addWidget(createDefaultAppsExpand(Browser));
    scrollLayout->addWidget(createDefaultAppsExpand(Mail));
    scrollLayout->addWidget(createDefaultAppsExpand(Text));
    scrollLayout->addWidget(createDefaultAppsExpand(Music));
    scrollLayout->addWidget(createDefaultAppsExpand(Video));
    scrollLayout->addWidget(createDefaultAppsExpand(Picture));
    scrollLayout->addWidget(createDefaultAppsExpand(Terminal));
    scrollLayout->addWidget(autoPlayApplications);
    scrollLayout->addWidget(new DUI::DSeparatorHorizontal);
    scrollLayout->addWidget(createDefaultAppsExpand(CD_Audio));
    scrollLayout->addWidget(createDefaultAppsExpand(DVD_Video));
    scrollLayout->addWidget(createDefaultAppsExpand(MusicPlayer));
    scrollLayout->addWidget(createDefaultAppsExpand(Camera));
    scrollLayout->addWidget(createDefaultAppsExpand(Software));
    scrollLayout->addItem(vSpacer);
    scrollLayout->setSpacing(0);
    scrollLayout->setMargin(0);

    QWidget *scrollWidget = new QWidget;
    scrollWidget->setLayout(scrollLayout);
    scrollWidget->setFixedWidth(310);
/*
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(new DUI::DSeparatorHorizontal);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(scrollWidget);

    m_centralWidget->setLayout(mainLayout);
}

DefaultApps::~DefaultApps()
{    
    qDebug() << "~DefaultApps()";

    m_centralWidget->deleteLater();
}

QFrame* DefaultApps::getContent()
{
    return m_centralWidget;
}

DUI::DArrowLineExpand *DefaultApps::createDefaultAppsExpand(const DefaultApps::DefaultAppsCategory &category)
{
    DUI::DArrowLineExpand *defaultApps = new DUI::DArrowLineExpand;
    QWidget *appsList = new QWidget;
    defaultApps->setContent(appsList);

    switch (category)
    {
    case Browser:       defaultApps->setTitle(tr("Browser"));       break;
    case Mail:          defaultApps->setTitle(tr("Mail"));          break;
    case Text:          defaultApps->setTitle(tr("Text"));          break;
    case Music:         defaultApps->setTitle(tr("Music"));         break;
    case Video:         defaultApps->setTitle(tr("Video"));         break;
    case Picture:       defaultApps->setTitle(tr("Picture"));       break;
    case Terminal:      defaultApps->setTitle(tr("Terminal"));      break;
    case CD_Audio:      defaultApps->setTitle(tr("CD_Audio"));      break;
    case DVD_Video:     defaultApps->setTitle(tr("DVD_Video"));     break;
    case MusicPlayer:   defaultApps->setTitle(tr("MusicPlayer"));   break;
    case Camera:        defaultApps->setTitle(tr("Camera"));        break;
    case Software:      defaultApps->setTitle(tr("Software"));      break;
    }

    // for test, we have nothing to show.
    appsList->setFixedHeight(30);

    defaultApps->setExpand(false);

    return defaultApps;
}
