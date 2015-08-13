#include <QVBoxLayout>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dheaderline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dslider.h>

#include "monitor.h"
#include "monitorground.h"
#include "singlemonitorsettings.h"

DUI_USE_NAMESPACE

SingleMonitorSettings::SingleMonitorSettings(QString monitorDBusPath, QWidget *parent)
    : QFrame(parent)
{
    initUI();
}

void SingleMonitorSettings::initUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    DHeaderLine * headerLine = new DHeaderLine;
    headerLine->setFixedHeight(48);
    headerLine->setText("Display");

    mainLayout->addWidget(headerLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    Monitor * monitor = new Monitor;
    monitor->setText("hello");

    MonitorGround * background = new MonitorGround;
    background->addMonitor(monitor);

    mainLayout->addWidget(background);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // resolution
    DArrowLineExpand * resolutionExpand = new DArrowLineExpand;
    resolutionExpand->setTitle("Resolution");

    DButtonGrid * resolutionButtons = new DButtonGrid(2, 3);
    QStringList resolutions;
    resolutions << "8888x88888" << "99999x9999" << "767777x7777";
    resolutions << "3333x3333";
    resolutionButtons->addButtons(resolutions);

    resolutionExpand->setContent(resolutionButtons);

    mainLayout->addWidget(resolutionExpand);

    // rotation
    DArrowLineExpand * rotationExpand = new DArrowLineExpand;
    rotationExpand->setTitle("Resolution");

    DButtonGrid * rotationButtons = new DButtonGrid(2, 3);
    QStringList rotations;
    rotations << "8888x88888" << "99999x9999" << "767777x7777";
    rotations << "3333x3333";
    rotationButtons->addButtons(rotations);

    rotationExpand->setContent(rotationButtons);
    rotationExpand->setExpand(false);

    mainLayout->addWidget(rotationExpand);

    // brightness
    DHeaderLine * brightnessHeaderLine = new DHeaderLine;
    brightnessHeaderLine->setText("Brightness");

    DSlider * brightnessSlider = new DSlider(Qt::Horizontal, brightnessHeaderLine);
    brightnessSlider->setMinimumSize(180, 20);
    brightnessHeaderLine->layout()->addWidget(brightnessSlider);

    mainLayout->addWidget(brightnessHeaderLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);
}
