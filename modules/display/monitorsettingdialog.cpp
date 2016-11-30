#include "monitorsettingdialog.h"

#include <QVBoxLayout>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc;

MonitorSettingDialog::MonitorSettingDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_primary(true),

      m_model(model),
      m_monitor(model->primaryMonitor())
{
    Q_ASSERT(m_monitor);

    init();
    initPrimary();
}

MonitorSettingDialog::MonitorSettingDialog(Monitor *monitor, QWidget *parent)
    : QDialog(parent),

      m_primary(false),
      m_monitor(monitor)
{
    init();
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

    m_btnsLayout = new QHBoxLayout;
    m_btnsLayout->addWidget(m_rotateBtn);
    m_btnsLayout->addStretch();
    m_btnsLayout->setSpacing(0);
    m_btnsLayout->setMargin(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_resolutionsWidget);
    mainLayout->addWidget(m_lightSlider);
    mainLayout->addLayout(m_btnsLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QTimer *smallDelayTimer = new QTimer(this);
    smallDelayTimer->setSingleShot(true);
    smallDelayTimer->setInterval(1000);

    connect(m_monitor, &Monitor::xChanged, smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_monitor, &Monitor::yChanged, smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_monitor, &Monitor::wChanged, smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_monitor, &Monitor::hChanged, smallDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(smallDelayTimer, &QTimer::timeout, this, &MonitorSettingDialog::onMonitorRectChanged);

    setWindowTitle(m_monitor->name());
    setLayout(mainLayout);
    onMonitorModeListChanged(m_monitor->modeList());
    QTimer::singleShot(10, this, &MonitorSettingDialog::onMonitorRectChanged);
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

    // load other non-primary dialogs
    for (auto mon : m_model->monitorList())
    {
//        if (mon == m_monitor)
//            continue;

        MonitorSettingDialog *dialog = new MonitorSettingDialog(mon, this);
        dialog->show();
        m_otherDialogs.append(dialog);
    }
}

void MonitorSettingDialog::onMonitorRectChanged()
{
    QDialog::move(m_monitor->rect().center() - rect().center());
}

void MonitorSettingDialog::onMonitorModeListChanged(const QList<Resolution> &modeList)
{
    bool first = true;
    for (auto r : modeList)
    {
        const QString option = QString::number(r.width()) + "×" + QString::number(r.height());

        if (first)
        {
            first = false;
            m_resolutionsWidget->appendOption(option + tr(" (Recommended)"));
        }
        m_resolutionsWidget->appendOption(option);
    }
}
