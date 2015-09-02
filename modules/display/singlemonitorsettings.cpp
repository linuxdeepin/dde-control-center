#include <QVBoxLayout>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dbuttonlist.h>

#include "moduleheader.h"
#include "normallabel.h"
#include "genericlistitem.h"

#include "monitor.h"
#include "monitorground.h"
#include "singlemonitorsettings.h"
#include "titleandwidget.h"
#include "vlayoutwidget.h"

DUI_USE_NAMESPACE

SingleMonitorSettings::SingleMonitorSettings(DisplayInterface * dbusDisplay,
                                             const QList<MonitorInterface *> &list,
                                             QWidget *parent)
    : QFrame(parent),
      m_dbusMonitors(list),
      m_dbusDisplay(dbusDisplay)
{
    m_rotationMap[1] = tr("Normal");
    m_rotationMap[2] = tr("Rotate right");
    m_rotationMap[4] = tr("Upside down");
    m_rotationMap[8] = tr("Rotate left");

    initUI();
}

void SingleMonitorSettings::initUI()
{
    if(m_dbusMonitors.count() <=0 )
        return;

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    VLayoutWidget *enableMonitorList = NULL;
    DButtonList *primaryMonitorList = NULL;
    DBaseLine * brightnessHeaderLine = NULL;
    VLayoutWidget *brightnessList = NULL;
    DArrowLineExpand *brightnessExpand = NULL;

    if(m_dbusMonitors.count() > 1){
        DArrowLineExpand * enableMonitor = new DArrowLineExpand;
        enableMonitor->setTitle(tr("Enable Monitor"));

        enableMonitorList = new VLayoutWidget;
        TitleAndWidget *enableTitle = new TitleAndWidget(enableMonitorList);
        enableTitle->setText(tr("Please select the monitor you want to enable (checkable)"));
        enableTitle->setFixedSize(290, m_dbusMonitors.count() * 25 + 20);
        enableMonitor->setContent(enableTitle);

        DArrowLineExpand * primaryMonitor = new DArrowLineExpand;
        primaryMonitor->setTitle(tr("Primary"));

        primaryMonitorList = new DButtonList;
        primaryMonitorList->setItemWidth(290);
        primaryMonitor->setContent(primaryMonitorList);

        mainLayout->addWidget(enableMonitor);
        mainLayout->addWidget(primaryMonitor);

        brightnessList = new VLayoutWidget;

        brightnessExpand = new DArrowLineExpand;
        brightnessExpand->setTitle(tr("Brightness"));
        brightnessExpand->setContent(brightnessList);
    }else{
        brightnessHeaderLine = new DBaseLine;

        NormalLabel * brightnessTitle = new NormalLabel(tr("Brightness"));
        brightnessHeaderLine->setLeftContent(brightnessTitle);
    }

    DArrowLineExpand * resolutionExpand = new DArrowLineExpand;
    resolutionExpand->setTitle(tr("Resolution"));

    DArrowLineExpand * rotationExpand = new DArrowLineExpand;
    rotationExpand->setTitle(("Rotation"));

    VLayoutWidget *resolutionList = new VLayoutWidget;

    VLayoutWidget *rotationList = new VLayoutWidget;

    foreach (MonitorInterface* dbusMonitor, m_dbusMonitors) {
        if(enableMonitorList){
            GenericListItem *enableMonitorItem = new GenericListItem;
            enableMonitorItem->setTitle(dbusMonitor->name());
            enableMonitorList->addWidget(enableMonitorItem, 25, Qt::AlignLeft);

            primaryMonitorList->addButton(dbusMonitor->name());
            primaryMonitorList->setFixedHeight(primaryMonitorList->count()*MENU_ITEM_HEIGHT);
        }

        DButtonGrid *resolutionButtons = new DButtonGrid;
        resolutionButtons->setItemSize(90, 30);
        QStringList resolutions = getResolutionLabels(dbusMonitor);
        resolutionButtons->setColumnCount(3);
        resolutionButtons->setRowCount(qCeil(resolutions.length() / 3.0));
        resolutionButtons->addButtons(resolutions);
        resolutionList->addWidget(new TitleAndWidget(resolutionButtons, tr("Monitor %1").arg(dbusMonitor->name())),
                                resolutionButtons->rowCount() * 30 + 30);
        resolutionExpand->setContent(resolutionList);

        updateResolutionButtons(dbusMonitor, resolutionButtons);

        DButtonGrid *rotationButtons = new DButtonGrid;
        rotationButtons->setItemSize(90, 30);
        QStringList rotations = getRotationLabels(dbusMonitor);
        rotationButtons->setColumnCount(2);
        rotationButtons->setRowCount(qCeil(rotations.length() / 2.0));
        rotationButtons->addButtons(rotations);
        rotationList->addWidget(new TitleAndWidget(rotationButtons, tr("Monitor %1").arg(dbusMonitor->name())),
                              rotationButtons->rowCount() * 30 + 30);
        rotationExpand->setContent(rotationList);

        updateRotationButtons(dbusMonitor, rotationButtons);

        DSlider *brightnessSlider = new DSlider(Qt::Horizontal, brightnessHeaderLine);
        brightnessSlider->setSingleStep(1);
        brightnessSlider->setPageStep(1);
        brightnessSlider->setRange(0, 10);
        if(brightnessHeaderLine){
            brightnessSlider->setMinimumWidth(180);
            brightnessHeaderLine->setRightContent(brightnessSlider);
        }else{
            brightnessSlider->setMinimumWidth(290);
            brightnessList->addWidget(new TitleAndWidget(brightnessSlider, tr("Monitor %1").arg(dbusMonitor->name())), 45);
        }
        updateBrightnessSlider(dbusMonitor, brightnessSlider);

        connect(brightnessSlider, &DSlider::valueChanged, [=]{
            m_dbusDisplay->SetBrightness(dbusMonitor->name(), brightnessSlider->value() / 10.0);
        });
        connect(m_dbusDisplay, &DisplayInterface::BrightnessChanged, [=]{
            updateBrightnessSlider(dbusMonitor, brightnessSlider);
        });

        connect(dbusMonitor, &MonitorInterface::CurrentModeChanged, [=]{
            updateResolutionButtons(dbusMonitor, resolutionButtons);
        });
        connect(resolutionButtons, &DButtonGrid::buttonCheckedIndexChanged, [=](int index){
            QDBusPendingReply<MonitorModeList> modesReply = dbusMonitor->ListModes();
            modesReply.waitForFinished();
            MonitorModeList monitorModeList = modesReply.value();
            dbusMonitor->SetMode(monitorModeList.at(index).id);
        });

        connect(dbusMonitor, &MonitorInterface::RotationChanged, [=]{
            updateRotationButtons(dbusMonitor, rotationButtons);
        });
        connect(rotationButtons, &DButtonGrid::buttonCheckedIndexChanged, [=](int index){
            ushort rotaion = dbusMonitor->rotation();
            foreach (ushort r, m_rotationMap.keys()) {
                if (m_rotationMap.value(r) == rotations.at(index)) {
                    rotaion = r;
                }
            }
            dbusMonitor->SetRotation(rotaion);
        });
    }

    mainLayout->addWidget(resolutionExpand);
    mainLayout->addWidget(rotationExpand);
    if(brightnessHeaderLine)
        mainLayout->addWidget(brightnessHeaderLine);
    else
        mainLayout->addWidget(brightnessExpand);
    mainLayout->addStretch(1);
}

void SingleMonitorSettings::updateResolutionButtons(MonitorInterface *dbus, DButtonGrid *resolutionButtons)
{
    QStringList resolutions = getResolutionLabels(dbus);

    MonitorMode currentMode = dbus->currentMode();
    QString currentResolution = QString("%1x%2").arg(currentMode.width).arg(currentMode.height);
    int index = resolutions.indexOf(currentResolution);
    resolutionButtons->checkButtonByIndex(index);
}

void SingleMonitorSettings::updateRotationButtons(MonitorInterface *dbus, DButtonGrid *rotationButtons)
{
    QStringList rotations = getRotationLabels(dbus);

    ushort currentRotation = dbus->rotation();
    int index = rotations.indexOf(m_rotationMap[currentRotation]);
    if(index >= 0)
        rotationButtons->checkButtonByIndex(index);
}

void SingleMonitorSettings::updateBrightnessSlider(MonitorInterface *dbus, DSlider *brightnessSlider)
{
    BrightnessMap brightnessMap = m_dbusDisplay->brightness();
    brightnessSlider->setValue(brightnessMap[dbus->name()] * 10);
}

QStringList SingleMonitorSettings::getResolutionLabels(MonitorInterface *dbus)
{
    QStringList resolutions;

    QDBusPendingReply<MonitorModeList> modesReply = dbus->ListModes();
    modesReply.waitForFinished();
    MonitorModeList monitorModeList = modesReply.value();

    foreach (MonitorMode mode, monitorModeList) {
        resolutions << QString("%1x%2").arg(mode.width).arg(mode.height);
    }

    return resolutions;
}

QStringList SingleMonitorSettings::getRotationLabels(MonitorInterface *dbus)
{
    QStringList rotations;

    QDBusPendingReply<UshortList> rotationsReply = dbus->ListRotations();
    rotationsReply.waitForFinished();
    UshortList monitorRotations = rotationsReply.value();

    foreach (ushort rotationId, monitorRotations) {
        rotations << m_rotationMap[rotationId];
    }

    return rotations;
}
