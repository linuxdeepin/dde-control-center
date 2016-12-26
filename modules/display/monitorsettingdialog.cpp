#include "monitorsettingdialog.h"
#include "monitorcontrolwidget.h"
#include "displaymodel.h"
#include "settingslistwidget.h"

#include <QVBoxLayout>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::display;

const double BRIGHTNESS_MUL = 1000.;

MonitorSettingDialog::MonitorSettingDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_primary(true),

      m_model(model),
      m_monitor(nullptr),

      m_smallDelayTimer(new QTimer(this))
{
    init();
    reloadMonitorObject(model->primaryMonitor());
    initPrimary();
}

MonitorSettingDialog::MonitorSettingDialog(Monitor *monitor, QWidget *parent)
    : QDialog(parent),

      m_primary(false),
      m_monitor(nullptr),

      m_smallDelayTimer(new QTimer(this))
{
    init();
    reloadMonitorObject(monitor);
}

MonitorSettingDialog::~MonitorSettingDialog()
{
    qDeleteAll(m_otherDialogs);
}

void MonitorSettingDialog::init()
{
    m_resolutionsWidget = new SettingsListWidget;
    m_resolutionsWidget->setTitle(tr("Resolution"));

    m_rotateBtn = new DImageButton;

    m_lightSlider = new DCCSlider;
    m_lightSlider->setOrientation(Qt::Horizontal);
    m_lightSlider->setMinimum(0);
    m_lightSlider->setMaximum(1000);
    m_lightSlider->setFixedWidth(350);

    m_btnsLayout = new QHBoxLayout;
    m_btnsLayout->addWidget(m_rotateBtn);
    m_btnsLayout->addStretch();
    m_btnsLayout->setSpacing(0);
    m_btnsLayout->setMargin(0);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_resolutionsWidget);
    m_mainLayout->addWidget(m_lightSlider);
    m_mainLayout->addLayout(m_btnsLayout);
    m_mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    m_smallDelayTimer->setSingleShot(true);
    m_smallDelayTimer->setInterval(1000);

    connect(m_resolutionsWidget, &SettingsListWidget::clicked, this, &MonitorSettingDialog::onMonitorModeSelected);
    connect(m_lightSlider, &DCCSlider::valueChanged, this, &MonitorSettingDialog::onBrightnessSliderChanged);
    connect(m_rotateBtn, &DImageButton::clicked, this, &MonitorSettingDialog::onRotateBtnClicked);
    connect(m_smallDelayTimer, &QTimer::timeout, this, &MonitorSettingDialog::onMonitorRectChanged);

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setLayout(m_mainLayout);
}

void MonitorSettingDialog::initPrimary()
{
    Q_ASSERT(m_primary);

    QPushButton *cancelBtn = new QPushButton;
    cancelBtn->setText(tr("Cancel"));
    QPushButton *applyBtn = new QPushButton;
    applyBtn->setText(tr("Apply"));

    m_btnsLayout->addWidget(cancelBtn);
    m_btnsLayout->addWidget(applyBtn);

    // add primary screen settings widget
    m_primarySettingsWidget = new SettingsListWidget;
    m_primarySettingsWidget->setTitle(tr("Primary"));
    m_mainLayout->insertWidget(0, m_primarySettingsWidget);

    m_ctrlWidget = new MonitorControlWidget;
    m_ctrlWidget->setDisplayModel(m_model);
    m_mainLayout->insertWidget(0, m_ctrlWidget);

    // add primary settings
    for (auto mon : m_model->monitorList())
        m_primarySettingsWidget->appendOption(mon->name());

    connect(m_ctrlWidget, &MonitorControlWidget::requestRecognize, this, &MonitorSettingDialog::requestRecognize);
    connect(m_ctrlWidget, &MonitorControlWidget::requestMerge, this, &MonitorSettingDialog::requestMerge);
    connect(m_ctrlWidget, &MonitorControlWidget::requestSplit, this, &MonitorSettingDialog::requestSplit);
    connect(m_ctrlWidget, &MonitorControlWidget::requestSetMonitorPosition, this, &MonitorSettingDialog::requestSetMonitorPosition);
    connect(m_primarySettingsWidget, &SettingsListWidget::clicked, this, &MonitorSettingDialog::requestSetPrimary);
    connect(m_model, &DisplayModel::primaryScreenChanged, this, &MonitorSettingDialog::onPrimaryChanged);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &MonitorSettingDialog::updateScreensRelation, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &MonitorSettingDialog::updateScreensRelation, Qt::QueuedConnection);
    connect(cancelBtn, &QPushButton::clicked, this, &MonitorSettingDialog::reject);
    connect(applyBtn, &QPushButton::clicked, this, &MonitorSettingDialog::accept);

    onPrimaryChanged();
    updateScreensRelation();

    reloadOtherScreensDialog();
}

void MonitorSettingDialog::reloadMonitorObject(Monitor *monitor)
{
    // only refersh mode list
    if (m_monitor == monitor)
        return onMonitorModeChanged();

    if (m_monitor)
    {
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged);
        disconnect(m_monitor, &Monitor::brightnessChanged, this, &MonitorSettingDialog::onMonitorBrightnessChanegd);
        disconnect(m_monitor, &Monitor::geometryChanged, m_smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    }

    m_monitor = monitor;

    connect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged, Qt::QueuedConnection);
    connect(m_monitor, &Monitor::brightnessChanged, this, &MonitorSettingDialog::onMonitorBrightnessChanegd);
    connect(m_monitor, &Monitor::geometryChanged, m_smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

    setWindowTitle(m_monitor->name());
    onMonitorModeChanged();
    onMonitorBrightnessChanegd(m_monitor->brightness());
    QTimer::singleShot(10, this, &MonitorSettingDialog::onMonitorRectChanged);
}

void MonitorSettingDialog::reloadOtherScreensDialog()
{
    qDeleteAll(m_otherDialogs);
    m_otherDialogs.clear();

    // load other non-primary dialogs
    for (auto mon : m_model->monitorList())
    {
        if (mon == m_monitor)
            continue;

        MonitorSettingDialog *dialog = new MonitorSettingDialog(mon, this);

        connect(dialog, &MonitorSettingDialog::requestSetPrimary, this, &MonitorSettingDialog::requestSetPrimary);
        connect(dialog, &MonitorSettingDialog::requestSetMonitorMode, this, &MonitorSettingDialog::requestSetMonitorMode);
        connect(dialog, &MonitorSettingDialog::requestSetMonitorBrightness, this, &MonitorSettingDialog::requestSetMonitorBrightness);
        connect(dialog, &MonitorSettingDialog::requestMonitorRotate, this, &MonitorSettingDialog::requestMonitorRotate);

        dialog->show();
        m_otherDialogs.append(dialog);
    }
}

void MonitorSettingDialog::updateScreensRelation()
{
    const bool merged = m_model->monitorsIsIntersect();

    m_ctrlWidget->setScreensMerged(merged);
    m_primarySettingsWidget->setVisible(!merged);

    for (auto d : m_otherDialogs)
        d->setVisible(!merged);

    onMonitorModeChanged();
}

void MonitorSettingDialog::onPrimaryChanged()
{
    Q_ASSERT(m_primary);

    // update current index
    const QString primary = m_model->primary();
    for (int i(0); i != m_model->monitorList().size(); ++i)
    {
        if (m_model->monitorList()[i]->name() == primary)
        {
            m_primarySettingsWidget->setSelectedIndex(i);
            break;
        }
    }

    if (m_monitor == m_model->primaryMonitor())
        return;

    reloadMonitorObject(m_model->primaryMonitor());
    reloadOtherScreensDialog();
}

void MonitorSettingDialog::onMonitorRectChanged()
{
    QDialog::move(m_monitor->rect().center() - rect().center());
}

void MonitorSettingDialog::onMonitorModeChanged()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();
    if (intersect)
        updateModeList(m_model->monitorsSameModeList());
    else
        updateModeList(m_monitor->modeList());

    if (!intersect)
        m_resolutionsWidget->setSelectedIndex(m_monitor->modeList().indexOf(m_monitor->currentMode()));
    else
    {
        const int w = m_model->screenWidth();
        const int h = m_model->screenHeight();
        const auto list = m_model->monitorsSameModeList();
        for (int i(0); i != list.size(); ++i)
        {
            if (list[i].width() == w && list[i].height() == h)
            {
                m_resolutionsWidget->setSelectedIndex(i);
                break;
            }
        }
    }
}

void MonitorSettingDialog::onMonitorBrightnessChanegd(const double brightness)
{
    m_lightSlider->blockSignals(true);
    m_lightSlider->setValue(brightness * BRIGHTNESS_MUL);
    m_lightSlider->blockSignals(false);
}

void MonitorSettingDialog::updateModeList(const QList<Resolution> &modeList)
{
    m_resolutionsWidget->clear();

    bool first = true;
    for (auto r : modeList)
    {
        const QString option = QString::number(r.width()) + "×" + QString::number(r.height());

        if (first)
        {
            first = false;
            m_resolutionsWidget->appendOption(option + tr(" (Recommended)"));
        } else {
            m_resolutionsWidget->appendOption(option);
        }
    }
}

void MonitorSettingDialog::onMonitorModeSelected(const int index)
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
    {
        const auto modeList = m_model->monitorsSameModeList();
        Q_ASSERT(modeList.size() > index);
        const auto mode = modeList[index];

        for (auto mon : m_model->monitorList())
        {
            const auto list = mon->modeList();
            for (int i(0); i != list.size(); ++i)
            {
                if (list[i].width() == mode.width() && list[i].height() == mode.height())
                    emit requestSetMonitorMode(mon, list[i].id());
            }
        }
    } else {
        const auto modeList = m_monitor->modeList();
        Q_ASSERT(modeList.size() > index);

        emit requestSetMonitorMode(m_monitor, modeList[index].id());
    }
}

void MonitorSettingDialog::onRotateBtnClicked()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
        emit requestMonitorRotate(nullptr);
    else
        emit requestMonitorRotate(m_monitor);
}

void MonitorSettingDialog::onBrightnessSliderChanged(const int value)
{
    const double v = value / BRIGHTNESS_MUL;
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
    {
        for (auto mon : m_model->monitorList())
            emit requestSetMonitorBrightness(mon, v);
    } else {
        emit requestSetMonitorBrightness(m_monitor, v);
    }
}
