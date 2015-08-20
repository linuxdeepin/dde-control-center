#include <QVBoxLayout>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>

#include "moduleheader.h"
#include "normallabel.h"

#include "monitor.h"
#include "monitorground.h"
#include "singlemonitorsettings.h"

DUI_USE_NAMESPACE

SingleMonitorSettings::SingleMonitorSettings(DisplayInterface * dbusDisplay, QString monitorDBusPath, QWidget *parent)
    : QFrame(parent)
{
    m_dbusMonitor = new MonitorInterface(monitorDBusPath, this);
    m_dbusDisplay = dbusDisplay;

    m_rotationMap[1] = "Normal";
    m_rotationMap[2] = "Rotate right";
    m_rotationMap[4] = "Upside down";
    m_rotationMap[8] = "Rotate left";

    initBackend();
    initUI();
}

void SingleMonitorSettings::initUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    ModuleHeader * headerLine = new ModuleHeader("Display");

    mainLayout->addWidget(headerLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    m_monitor = new Monitor;
    m_monitorGround = new MonitorGround;
    m_monitorGround->addMonitor(m_monitor);
    updateMonitor();

    mainLayout->addWidget(m_monitorGround);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // resolution
    DArrowLineExpand * resolutionExpand = new DArrowLineExpand;
    resolutionExpand->setTitle("Resolution");

    m_resolutionButtons = new DButtonGrid;
    QStringList resolutions = getResolutionLabels();
    m_resolutionButtons->setColumnCount(3);
    m_resolutionButtons->setRowCount(qCeil(resolutions.length() / 3.0));
    m_resolutionButtons->addButtons(resolutions);
    updateResolutionButtons();

    connect(m_dbusMonitor, &MonitorInterface::CurrentModeChanged, this, &SingleMonitorSettings::updateResolutionButtons);
    connect(m_resolutionButtons, &DButtonGrid::buttonCheckedIndexChanged, [=](int index){
        m_dbusMonitor->SetMode(m_monitorModeList.at(index).id);
        m_dbusDisplay->Apply();
    });

    resolutionExpand->setContent(m_resolutionButtons);
    mainLayout->addWidget(resolutionExpand);

    // rotation
    DArrowLineExpand * rotationExpand = new DArrowLineExpand;
    rotationExpand->setTitle("Resolution");

    m_rotationButtons = new DButtonGrid;
    QStringList rotations = getRotationLabels();
    m_rotationButtons->setColumnCount(2);
    m_rotationButtons->setRowCount(qCeil(rotations.length() / 2.0));
    m_rotationButtons->addButtons(rotations);
    updateRotationButtons();

    connect(m_dbusMonitor, &MonitorInterface::RotationChanged, this, &SingleMonitorSettings::updateRotationButtons);
    connect(m_rotationButtons, &DButtonGrid::buttonCheckedIndexChanged, [=](int index){
        ushort rotaion = m_dbusMonitor->rotation();
        foreach (ushort r, m_rotationMap.keys()) {
            if (m_rotationMap.value(r) == rotations.at(index)) {
                rotaion = r;
            }
        }
        m_dbusMonitor->SetRotation(rotaion);
        m_dbusDisplay->Apply();
    });

    rotationExpand->setContent(m_rotationButtons);
    mainLayout->addWidget(rotationExpand);

    // brightness
    DBaseLine * brightnessHeaderLine = new DBaseLine;

    NormalLabel * brightnessTitle = new NormalLabel("Brightness");
    brightnessHeaderLine->setLeftContent(brightnessTitle);

    m_brightnessSlider = new DSlider(Qt::Horizontal, brightnessHeaderLine);
    m_brightnessSlider->setMinimumWidth(180);
    m_brightnessSlider->setSingleStep(1);
    m_brightnessSlider->setPageStep(1);
    brightnessHeaderLine->setRightContent(m_brightnessSlider);
    m_brightnessSlider->setRange(0, 10);
    updateBrightnessSlider();

    connect(m_brightnessSlider, &DSlider::valueChanged, [=]{
        m_dbusDisplay->SetBrightness(m_dbusMonitor->name(), m_brightnessSlider->value() / 10.0);
    });
    connect(m_dbusDisplay, &DisplayInterface::BrightnessChanged, this, &SingleMonitorSettings::updateBrightnessSlider);

    mainLayout->addWidget(brightnessHeaderLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);
}

void SingleMonitorSettings::initBackend()
{
    QDBusPendingReply<MonitorModeList> modesReply = m_dbusMonitor->ListModes();
    modesReply.waitForFinished();
    m_monitorModeList = modesReply.value();

    QDBusPendingReply<UshortList> rotationsReply = m_dbusMonitor->ListRotations();
    rotationsReply.waitForFinished();
    m_monitorRotations = rotationsReply.value();
}

void SingleMonitorSettings::updateMonitor()
{
    m_monitor->setName(m_dbusMonitor->name());
    m_monitor->setResolution(m_dbusMonitor->width(), m_dbusMonitor->height());
    m_monitorGround->relayout();
}

void SingleMonitorSettings::updateResolutionButtons()
{
    QStringList resolutions = getResolutionLabels();

    MonitorMode currentMode = m_dbusMonitor->currentMode();
    QString currentResolution = QString("%1x%2").arg(currentMode.width).arg(currentMode.height);
    m_resolutionButtons->checkButtonByIndex(resolutions.indexOf(currentResolution));

    updateMonitor();
}

void SingleMonitorSettings::updateRotationButtons()
{
    QStringList rotations = getRotationLabels();

    ushort currentRotation = m_dbusMonitor->rotation();
    m_rotationButtons->checkButtonByIndex(rotations.indexOf(m_rotationMap[currentRotation]));

    updateMonitor();
}

void SingleMonitorSettings::updateBrightnessSlider()
{
    BrightnessMap brightnessMap = m_dbusDisplay->brightness();
    m_brightnessSlider->setValue(brightnessMap[m_dbusMonitor->name()] * 10);
}

QStringList SingleMonitorSettings::getResolutionLabels()
{
    QStringList resolutions;

    foreach (MonitorMode mode, m_monitorModeList) {
        resolutions << QString("%1x%2").arg(mode.width).arg(mode.height);
    }

    return resolutions;
}

QStringList SingleMonitorSettings::getRotationLabels()
{
    QStringList rotations;

    foreach (ushort rotationId, m_monitorRotations) {
        rotations << m_rotationMap[rotationId];
    }

    return rotations;
}
