#include "monitorsettingdialog.h"
#include "monitorcontrolwidget.h"
#include "displaymodel.h"
#include "settingslistwidget.h"
#include "basiclistmodel.h"
#include "basiclistview.h"
#include "basiclistdelegate.h"
#include "dthememanager.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {

namespace display {

MonitorSettingDialog::MonitorSettingDialog(DisplayModel *model, QWidget *parent)
    : DAbstractDialog(parent),

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
    : DAbstractDialog(parent),

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

void MonitorSettingDialog::resizeEvent(QResizeEvent *e)
{
    DAbstractDialog::resizeEvent(e);

    QTimer::singleShot(1, this, &MonitorSettingDialog::onMonitorRectChanged);
}

void MonitorSettingDialog::mouseMoveEvent(QMouseEvent *e)
{
    e->ignore();
}

void MonitorSettingDialog::init()
{
    DThemeManager::instance()->setTheme(this, "light");

    setBackgroundColor(QColor(Qt::white));
    setBorderColor(QColor(0, 0, 0, 0.2 * 255));

    m_resolutionsModel = new BasicListModel;

    BasicListView *resolutionView = new BasicListView;
    resolutionView->setModel(m_resolutionsModel);
    resolutionView->setItemDelegate(new BasicListDelegate);

    connect(resolutionView, &BasicListView::entered, m_resolutionsModel, &BasicListModel::setHoveredIndex);

    if (m_primary)
    {
        resolutionView->setAutoFitHeight(false);
        resolutionView->setFixedHeight(36 * 3);
    }

    resolutionView->setMinimumWidth(448);

    QLabel *resoLabel = new QLabel;
    resoLabel->setObjectName("Resolution");
    resoLabel->setText(tr("Resolution"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addSpacing(30);
    hlayout->addWidget(resoLabel);

    QVBoxLayout *resoLayout = new QVBoxLayout;
    resoLayout->addLayout(hlayout);
    resoLayout->addWidget(resolutionView);
    resoLayout->setSpacing(5);
    resoLayout->setContentsMargins(10, 0, 10, 0);

#ifndef DCC_DISABLE_ROTATE
    m_rotateBtn = new DImageButton;
    m_rotateBtn->setNormalPic(":/display/themes/dark/icons/rotate_normal.png");
    m_rotateBtn->setHoverPic(":/display/themes/dark/icons/rotate_hover.png");
    m_rotateBtn->setPressPic(":/display/themes/dark/icons/rotate_press.png");
#endif

    m_monitorName = new QLabel;
    m_monitorName->setAlignment(Qt::AlignCenter);
    m_monitorName->setObjectName("MonitorName");

//    m_lightSlider = new DCCSlider;
//    m_lightSlider->setOrientation(Qt::Horizontal);
//    m_lightSlider->setMinimum(0);
//    m_lightSlider->setMaximum(1000);
//    m_lightSlider->setMinimumWidth(430);
//    m_lightSlider->setTracking(true);

    QLabel *minLight = new QLabel;
    minLight->setPixmap(QPixmap(":/display/themes/dark/icons/brightness_low.png"));
    QLabel *maxLight = new QLabel;
    maxLight->setPixmap(QPixmap(":/display/themes/dark/icons/brightness_high.png"));

//    QHBoxLayout *lightLayout = new QHBoxLayout;
//    lightLayout->addStretch();
//    lightLayout->addWidget(minLight);
//    lightLayout->addWidget(m_lightSlider);
//    lightLayout->addWidget(maxLight);
//    lightLayout->addStretch();
//    lightLayout->setSpacing(10);
//    lightLayout->setContentsMargins(10, 0, 10, 0);

    m_btnsLayout = new QHBoxLayout;
    m_btnsLayout->addSpacing(15);
#ifndef DCC_DISABLE_ROTATE
    m_btnsLayout->addWidget(m_rotateBtn);
#endif
    m_btnsLayout->addStretch();
    m_btnsLayout->setSpacing(10);
    m_btnsLayout->setContentsMargins(10, 0, 10, 0);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addWidget(m_monitorName);
    m_mainLayout->addLayout(resoLayout);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(m_btnsLayout);
    m_mainLayout->addSpacing(10);

    QWidget *widget = new QWidget;
    widget->setLayout(m_mainLayout);

    setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);

    m_smallDelayTimer->setSingleShot(true);
    m_smallDelayTimer->setInterval(1000);

    connect(resolutionView, &BasicListView::clicked, [=](const QModelIndex &index) { onMonitorModeSelected(index.row()); });
//    connect(m_lightSlider, &DCCSlider::valueChanged, this, &MonitorSettingDialog::onBrightnessSliderChanged);
#ifndef DCC_DISABLE_ROTATE
    connect(m_rotateBtn, &DImageButton::clicked, this, &MonitorSettingDialog::onRotateBtnClicked);
#endif
    connect(m_smallDelayTimer, &QTimer::timeout, this, &MonitorSettingDialog::onMonitorRectChanged);
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
    m_mainLayout->insertWidget(1, m_primarySettingsWidget);

    m_ctrlWidget = new MonitorControlWidget;
    m_ctrlWidget->setDisplayModel(m_model);

    m_mainLayout->insertWidget(1, m_ctrlWidget);

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
    connect(m_model, &DisplayModel::displayModeChanged, this, &MonitorSettingDialog::reject);
    connect(cancelBtn, &QPushButton::clicked, this, &MonitorSettingDialog::reject);
    connect(applyBtn, &QPushButton::clicked, this, &MonitorSettingDialog::accept);

    reloadOtherScreensDialog();

    onPrimaryChanged();
    QTimer::singleShot(1, this, &MonitorSettingDialog::updateScreensRelation);

    applyBtn->setFocus();
}

void MonitorSettingDialog::reloadMonitorObject(Monitor *monitor)
{
    // only refersh mode list
    if (m_monitor == monitor)
        return onMonitorModeChanged();

    if (m_monitor)
    {
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged);
//        disconnect(m_monitor, &Monitor::brightnessChanged, this, &MonitorSettingDialog::onMonitorBrightnessChanegd);
        disconnect(m_monitor, &Monitor::geometryChanged, m_smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    }

    m_monitor = monitor;

    connect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged, Qt::QueuedConnection);
//    connect(m_monitor, &Monitor::brightnessChanged, this, &MonitorSettingDialog::onMonitorBrightnessChanegd);
    connect(m_monitor, &Monitor::geometryChanged, m_smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

    m_monitorName->setText(m_monitor->name());
    setWindowTitle(m_monitor->name());
    onMonitorModeChanged();
//    onMonitorBrightnessChanegd(m_monitor->brightness());
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
//        connect(dialog, &MonitorSettingDialog::requestSetMonitorBrightness, this, &MonitorSettingDialog::requestSetMonitorBrightness);
#ifndef DCC_DISABLE_ROTATE
        connect(dialog, &MonitorSettingDialog::requestMonitorRotate, this, &MonitorSettingDialog::requestMonitorRotate);
#endif
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

    adjustSize();
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
    DAbstractDialog::move(m_monitor->rect().center() - rect().center());
}

void MonitorSettingDialog::onMonitorModeChanged()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();
    if (intersect)
        updateModeList(m_model->monitorsSameModeList());
    else
        updateModeList(m_monitor->modeList());

    if (!intersect)
    {
        m_resolutionsModel->setSelectedIndex(m_resolutionsModel->index(m_monitor->modeList().indexOf(m_monitor->currentMode())));
    }
    else
    {
        const int w = m_model->screenWidth();
        const int h = m_model->screenHeight();
        const auto list = m_model->monitorsSameModeList();
        for (int i(0); i != list.size(); ++i)
        {
            if (list[i].width() == w && list[i].height() == h)
            {
                m_resolutionsModel->setSelectedIndex(m_resolutionsModel->index(i));
                break;
            }
        }
    }
}

//void MonitorSettingDialog::onMonitorBrightnessChanegd(const double brightness)
//{
//    m_lightSlider->blockSignals(true);
//    m_lightSlider->setValue(brightness * BRIGHTNESS_MUL);
//    m_lightSlider->blockSignals(false);
//}

void MonitorSettingDialog::updateModeList(const QList<Resolution> &modeList)
{
    m_resolutionsModel->clear();

    bool first = true;
    for (auto r : modeList)
    {
        const QString option = QString::number(r.width()) + "×" + QString::number(r.height());

        if (first)
        {
            first = false;
            m_resolutionsModel->appendOption(option + tr(" (Recommended)"));
        } else {
            m_resolutionsModel->appendOption(option);
        }
    }

    emit m_resolutionsModel->layoutChanged();
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

#ifndef DCC_DISABLE_ROTATE
void MonitorSettingDialog::onRotateBtnClicked()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
        emit requestMonitorRotate(nullptr);
    else
        emit requestMonitorRotate(m_monitor);
}
#endif

//void MonitorSettingDialog::onBrightnessSliderChanged(const int value)
//{
//    const double v = value / BRIGHTNESS_MUL;
//    const bool intersect = m_primary && m_model->monitorsIsIntersect();

//    if (intersect)
//    {
//        for (auto mon : m_model->monitorList())
//            emit requestSetMonitorBrightness(mon, v);
//    } else {
//        emit requestSetMonitorBrightness(m_monitor, v);
//    }
//}

} // namespace display

} // namespace dcc
