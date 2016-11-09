#include "speedslider.h"

DWIDGET_USE_NAMESPACE

SpeedSlider::SpeedSlider(QObject *parent)
{
    Q_UNUSED(parent);
    m_mainLayout = new QVBoxLayout;
    m_speedSlider = new QSlider(Qt::Horizontal, this);
    m_title = new QLabel;
    m_speedSlider->setOrientation(Qt::Horizontal);
    m_speedSlider->setTracking(false);
    m_speedSlider->setTickPosition(QSlider::TicksBothSides);

    m_mainLayout->addWidget(m_title);
    m_mainLayout->addWidget(m_speedSlider);
    QHBoxLayout *hboxlayout = new QHBoxLayout;
    QLabel *leftValue = new QLabel;
    QLabel *rightValue = new QLabel;
    leftValue->setText(tr("Slow"));
    rightValue->setText(tr("Fase"));
    hboxlayout->addWidget(leftValue);
    hboxlayout->addStretch();
    hboxlayout->addWidget(rightValue);
    m_mainLayout->addLayout(hboxlayout);
    setLayout(m_mainLayout);
    connect(m_speedSlider, &QSlider::valueChanged, this, &SpeedSlider::requestSetSliderValue);
}

void SpeedSlider::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SpeedSlider::setValue(const int &value)
{
    m_speedSlider->blockSignals(true);
    m_speedSlider->setValue(value);
    m_speedSlider->blockSignals(false);
}

void SpeedSlider::setMaxValue(const int &value)
{
    m_speedSlider->setMaximum(value);
}

void SpeedSlider::setMinValue(const int &value)
{
    m_speedSlider->setMinimum(value);
}

void SpeedSlider::setStep(const int &value)
{
    m_speedSlider->setSingleStep(value);
    m_speedSlider->setPageStep(value);
    m_speedSlider->setTickInterval(value);
}
