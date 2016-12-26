#include "fontsizewidget.h"
#include "../../widget/fontitem.h"
#include "../../model/fontsizemodel.h"
#include "titledslideritem.h"
#include "dccslider.h"

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

FontSizeWidget::FontSizeWidget(QWidget *parent)
    :TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_mainWidget = new SettingsGroup;
    m_sizeWidget = new TitledSliderItem(tr("Size"));

    m_sizeWidget->slider()->setType(DCCSlider::Vernier);
    m_sizeWidget->slider()->setRange(-1, 1);
    m_sizeWidget->slider()->setTickPosition(QSlider::TicksBelow);
    m_sizeWidget->slider()->setTickInterval(1);
    m_sizeWidget->setObjectName("FontSizeWidget");
    m_mainWidget->appendItem(m_sizeWidget);
    m_mainlayout->addWidget(m_mainWidget);
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
    case -1:
        return "10px";
    case 0:
        return "12px";
    case 1:
        return "16px";
    default:
        return "12px";
    }
}

