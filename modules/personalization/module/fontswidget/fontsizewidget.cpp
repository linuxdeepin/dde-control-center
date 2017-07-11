#include "fontsizewidget.h"
#include "../../widget/fontitem.h"
#include "../../model/fontsizemodel.h"
#include "titledslideritem.h"
#include "dccslider.h"

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

FontSizeWidget::FontSizeWidget(QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_mainWidget = new SettingsGroup;
    m_sizeWidget = new TitledSliderItem(tr("Size"));

    m_sizeWidget->slider()->setType(DCCSlider::Vernier);
    m_sizeWidget->slider()->setRange(0, 6);
    m_sizeWidget->slider()->setTickPosition(QSlider::TicksBelow);
    m_sizeWidget->slider()->setTickInterval(1);
    m_sizeWidget->slider()->setPageStep(1);
    m_sizeWidget->setObjectName("FontSizeWidget");
    m_mainWidget->appendItem(m_sizeWidget);
    m_mainlayout->addWidget(m_mainWidget);

    m_mainlayout->setSpacing(0);
    m_mainlayout->setMargin(0);

    setLayout(m_mainlayout);

    m_fontSlider = m_sizeWidget->slider();
    connect(m_fontSlider, &QSlider::valueChanged, this, &FontSizeWidget::requestSetFontSize);
    connect(m_fontSlider, &QSlider::valueChanged, this, &FontSizeWidget::setFontSizeTip);
}

void FontSizeWidget::setModel(FontSizeModel *const model)
{
    connect(model, &FontSizeModel::sizeChanged, this, &FontSizeWidget::setFontSize);
    setFontSize(model->getFontSize());
}

void FontSizeWidget::setFontSize(int size)
{
    m_fontSlider->blockSignals(true);
    m_fontSlider->setValue(size);
    m_fontSlider->blockSignals(false);
    m_sizeWidget->setValueLiteral(delayToLiteralString(size));
}

void FontSizeWidget::setFontSizeTip(int size)
{
    m_sizeWidget->setValueLiteral(delayToLiteralString(size));
}

QString FontSizeWidget::delayToLiteralString(const int delay) const
{
    switch (delay) {
    case 0:
        return "11px";
    case 1:
        return "12px";
    case 2:
        return "13px";
    case 3:
        return "15px";
    case 4:
        return "16px";
    case 5:
        return "18px";
    case 6:
        return "20px";
    default:
        return "12px";
    }
}

