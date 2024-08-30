// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "scalingwidget.h"

#include "operation/displaymodel.h"
#include "operation/monitor.h"

#include <QVBoxLayout>

const float MinScreenWidth = 1024.0f;
const float MinScreenHeight = 768.0f;

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

ScalingWidget::ScalingWidget(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
    , m_tipWidget(new QWidget(this))
    , m_tipLabel(new DTipLabel(tr("The monitor only supports 100% display scaling"), this))
    , m_slider(new TitledSliderItem(QString(), this))
{
    // 初始化列表无法进行静态翻译
    m_title = new TitleLabel(tr("Display Scaling"), this);
    m_title->setText(tr("Display Scaling"));

    m_tipWidget->setAccessibleName("ScalingWidget_tipWidget");
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(8);
    m_centralLayout->addWidget(m_title);

    m_tipLabel->setForegroundRole(DPalette::TextTips);
    m_tipLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QVBoxLayout *tipLayout = new QVBoxLayout(m_tipWidget);
    tipLayout->setContentsMargins(10, 0, 0, 0);
    tipLayout->addWidget(m_tipLabel);
    m_tipWidget->setLayout(tipLayout);
    m_centralLayout->addWidget(m_tipWidget);

    m_slider->addBackground();
    m_centralLayout->addWidget(m_slider);
    setLayout(m_centralLayout);
}

ScalingWidget::~ScalingWidget() { }

void ScalingWidget::setModel(DisplayModel *model)
{
    m_displayModel = model;

    addSlider();
}

void ScalingWidget::addSlider()
{
    if (m_displayModel->monitorList().size() == 0)
        return;

    onResolutionChanged();
    DCCSlider *slider = m_slider->slider();
    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        if (value > 0 && value <= m_scaleList.size()
            && m_displayModel->uiScale() != m_scaleList[value - 1].toFloat()) {
            m_slider->setValueLiteral(QString("%1").arg(m_scaleList[value - 1].toFloat()));
            Q_EMIT requestUiScaleChange(m_scaleList[value - 1].toFloat());
        }
    });
    connect(m_displayModel, &DisplayModel::uiScaleChanged, this, [=](const double scale) {
        slider->blockSignals(true);
        qDebug() << "monitor scaleChanged ,scale :" << convertToSlider(scale);
        slider->setValue(convertToSlider(scale));
        slider->blockSignals(false);
    });

    for (auto moni : m_displayModel->monitorList()) {
        connect(moni, &Monitor::currentModeChanged, this, &ScalingWidget::onResolutionChanged);
        connect(moni, &Monitor::enableChanged, this, &ScalingWidget::onResolutionChanged);
    }
}

void ScalingWidget::onResolutionChanged()
{
    QStringList fscaleList = { "1.0", "1.25", "1.5", "1.75", "2.0", "2.25", "2.5", "2.75", "3.0" };
    for (auto moni : m_displayModel->monitorList()) {
        if (!moni->enable()) {
            continue;
        }
        auto tmode = moni->currentMode();
        // 后端传入currentMode值可能为0
        if (tmode.width() == 0 || tmode.height() == 0) {
            fscaleList.clear();
            break;
        }
        auto ts = getScaleList(tmode);
        fscaleList = ts.size() < fscaleList.size() ? ts : fscaleList;
    }

    // 如果仅一个缩放值可用
    if (fscaleList.size() <= 1) {
        fscaleList.clear();
        fscaleList.append(QStringList() << "1.0"
                                        << "1.0");
        m_tipWidget->setVisible(true);
    } else {
        m_tipWidget->setVisible(false);
    }

    m_scaleList = fscaleList;
    m_slider->setAnnotations(m_scaleList);
    DCCSlider *slider = m_slider->slider();
    slider->blockSignals(true);
    slider->setRange(1, m_scaleList.size());
    slider->setPageStep(1);
    slider->setValue(
            convertToSlider(m_displayModel->uiScale() > 1.0 ? m_displayModel->uiScale() : 1.0));
    slider->update();
    slider->blockSignals(false);
};

QStringList ScalingWidget::getScaleList(const Resolution &r)
{
    const QStringList tvstring = {
        "1.0", "1.25", "1.5", "1.75", "2.0", "2.25", "2.5", "2.75", "3.0"
    };
    QStringList fscaleList;
    auto maxWScale = r.width() / MinScreenWidth;
    auto maxHScale = r.height() / MinScreenHeight;
    auto maxScale = maxWScale < maxHScale ? maxWScale : maxHScale;
    maxScale = maxScale < 3.0f ? maxScale : 3.0f;
    for (int idx = 0; idx * 0.25f + 1.0f <= maxScale; ++idx) {
        fscaleList << tvstring[idx];
    }

    return fscaleList;
}

int ScalingWidget::convertToSlider(const double value)
{
    // remove base scale (100), then convert to 1-based value
    // with a stepping of 25
    return int(round(double(value * 100 - 100) / 25)) + 1;
}
