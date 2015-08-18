#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QObject>
#include <QDBusPendingReply>

#include "defaultapps.h"
#include "dbus/dbusdefaultapps.h"

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dbuttonlist.h>
#include <libdui/dswitchlineexpand.h>

DefaultApps::DefaultApps()
    : m_dbusDefaultApps("com.deepin.daemon", "/com/deepin/daemon/DefaultApps", QDBusConnection::sessionBus())
{
    Q_INIT_RESOURCE(dark);

    AppList::registerMetaType();
    AppType::registerMetaType();

    m_centralWidget = new QFrame;

    QSpacerItem *hSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //QSpacerItem *vSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
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

    DUI::DSwitchLineExpand *autoPlayApplications = new DUI::DSwitchLineExpand;
    autoPlayApplications->setTitle(tr("AutoPlay"));

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
    //scrollLayout->addItem(vSpacer);
    //scrollLayout->addWidget(new QWidget);
    scrollLayout->addStretch(1);
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
    m_centralWidget->updateGeometry();
    m_centralWidget->update();
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

    int index = 0;

    DUI::DButtonList *list = new DUI::DButtonList;
    list->setItemHeight(30);
    AppList appList = getAppsListByCategory(category);
    for (const AppType &i : appList.list)
        list->addButton(i.s2, index++);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(list);
    layout->setMargin(0);
    layout->setSpacing(0);

    QWidget *appsList = new QWidget;

    // for test, we have nothing to show.
    //appsList->setMinimumHeight(appsList->sizeHint().height());
    appsList->setFixedHeight(30 * appList.list.count());
    //list->setFixedSize(list->width(), 30 * appList.list.count());
    //appsList->setFixedHeight(50);
    appsList->setLayout(layout);

    defaultApps->setContent(appsList);
    defaultApps->setExpand(false);

    return defaultApps;
}

AppList DefaultApps::getAppsListByCategory(const DefaultApps::DefaultAppsCategory &category)
{
    QString type = "";

    switch (category)
    {
    case Browser:       type = "x-scheme-handler/http";     break;
    case Mail:          type = "x-scheme-handler/mailto";   break;
    case Text:          type = "text/plain";                break;
    case Music:         type = "audio/mpeg";                break;
    case Video:         type = "video/mp4";                 break;
    case Picture:       type = "image/jpeg";                break;
    case Terminal:      type = "terminal";                  break;
    case CD_Audio:      type = "x-content/audio-cdda";      break;
    case DVD_Video:     type = "x-content/video-dvd";       break;
    case MusicPlayer:   type = "x-content/audio-player";    break;
    case Camera:        type = "x-content/image-dcf";       break;
    case Software:      type = "x-content/unix-software";   break;
    }

    QDBusPendingReply<AppList> list = m_dbusDefaultApps.AppsListViaType(type);

    qDebug() << type << endl << list.argumentAt<0>().list;

    return list.argumentAt<0>();
}
