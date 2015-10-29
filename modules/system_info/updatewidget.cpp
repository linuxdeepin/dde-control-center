#include "updatewidget.h"
#include "separatorwidget.h"
#include "constants.h"
#include "dbus/appupdateinfo.h"

#include <QVBoxLayout>
#include <QResizeEvent>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
{
    D_THEME_INIT_WIDGET(UpdateWidget);

    m_dbusUpdateInter = new DBusLastoreManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);

    m_updateCountTips = new QLabel;
    m_updateCountTips->setObjectName("Tips");
    m_updateSizeTips = new QLabel;
    m_updateSizeTips->setObjectName("Tips");
    m_updateProgress = new DCircleProgress;
    m_updateProgress->setFixedSize(35, 35);
    m_updateProgress->setLineWidth(2);
    m_appsList = new DListWidget;
//    m_appsList->setSelectionMode(QListWidget::NoSelection);
//    m_appsList->setFrameStyle(QFrame::NoFrame);
    m_appsList->setFixedWidth(DCC::ModuleContentWidth);
    m_appsList->setItemSize(DCC::ModuleContentWidth, 50);
    m_appsList->setEnableVerticalScroll(true);
    m_appsList->setObjectName("AppList");
//    m_appsList->setStyleSheet(QString("background-color:#252627;"));
//    m_appItems = new QMap<QListWidgetItem *, ApplictionItemWidget *>;

    // TODO: remove
    m_updateCountTips->setText("You have 2 softwares need update");
    m_updateSizeTips->setText("Total update size: 130M");

    QVBoxLayout *tipsLayout = new QVBoxLayout;
    tipsLayout->addWidget(m_updateCountTips);
    tipsLayout->addWidget(m_updateSizeTips);
    tipsLayout->setSpacing(0);
    tipsLayout->setMargin(0);

    QHBoxLayout *updateInfoLayout = new QHBoxLayout;
    updateInfoLayout->addLayout(tipsLayout);
    updateInfoLayout->addStretch();
    updateInfoLayout->addWidget(m_updateProgress);
    updateInfoLayout->setSpacing(0);
    updateInfoLayout->setContentsMargins(10, 8, 10, 8);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(updateInfoLayout);
//    mainLayout->addWidget(new HSeparatorWidget);
    mainLayout->addWidget(m_appsList);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    loadAppList();
    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    //    setFixedHeight(200);
}

void UpdateWidget::resizeEvent(QResizeEvent *e)
{
    m_appsList->setMaximumHeight(e->size().height() - 55);
}

void UpdateWidget::loadAppList()
{
    // TODO: lang
    QList<AppUpdateInfo> updateInfoList = m_dbusUpdateInter->ApplicationUpdateInfos1("zh_CN").value();
    ApplictionItemWidget *appItemWidget;

    for (const AppUpdateInfo &info : updateInfoList)
    {
        appItemWidget = new ApplictionItemWidget;
        appItemWidget->setAppUpdateInfo(info);

        m_appsList->addWidget(appItemWidget);
    }


    for (int i = 0; i != 3; ++i)
        for (const AppUpdateInfo &info : updateInfoList)
        {
            appItemWidget = new ApplictionItemWidget;
            appItemWidget->setAppUpdateInfo(info);

            m_appsList->addWidget(appItemWidget);
        }

//    m_appsList->addWidget(new ApplictionItemWidget);
//    ApplictionItemWidget *appItem = new ApplictionItemWidget;
//    QListWidgetItem *widgetItem = new QListWidgetItem;

//    m_appsList->addWidget()
//    m_appsList->addItem(widgetItem);
//    m_appsList->setItemWidget(widgetItem, appItem);

//    m_appItems->insert(widgetItem, appItem);
}

