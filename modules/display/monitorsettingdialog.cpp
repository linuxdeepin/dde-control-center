#include "monitorsettingdialog.h"

#include <QVBoxLayout>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc;

MonitorSettingDialog::MonitorSettingDialog(DisplayModel *model, Monitor *monitor, QWidget *parent)
    : QDialog(parent),

      m_primary(false),

      m_model(model),
      m_monitor(monitor),

      m_rotateBtn(new DImageButton),
      m_lightSlider(new DCCSlider),
      m_btnsLayout(new QHBoxLayout)
{
    m_lightSlider->setOrientation(Qt::Horizontal);

    m_btnsLayout->addWidget(m_rotateBtn);
    m_btnsLayout->addStretch();
    m_btnsLayout->setSpacing(0);
    m_btnsLayout->setMargin(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
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

    setWindowTitle(monitor->name());
    setLayout(mainLayout);
}

void MonitorSettingDialog::setPrimary(const bool primary)
{
    Q_ASSERT(primary && !m_primary);
    m_primary = primary;

    QPushButton *cancelBtn = new QPushButton;
    cancelBtn->setText(tr("Cancel"));
    QPushButton *applyBtn = new QPushButton;
    applyBtn->setText(tr("Apply"));

    m_btnsLayout->addWidget(cancelBtn);
    m_btnsLayout->addWidget(applyBtn);
}

void MonitorSettingDialog::onMonitorRectChanged()
{
    const QRect r(m_monitor->x(), m_monitor->y(), m_monitor->w(), m_monitor->h());

    QDialog::move(r.center() - rect().center());
}
