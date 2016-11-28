#include "fontswidget.h"

FontsWidget::FontsWidget(QWidget *parent)
    :ContentWidget(parent)
{
    m_mainlayout         = new QVBoxLayout;
    m_widget             = new TranslucentFrame;
    m_fontSizeWidget     = new FontSizeWidget;
    m_fontSettingsWidget = new FontSettingsWidget;
    m_mainlayout->addWidget(m_fontSizeWidget);
    m_mainlayout->addWidget(m_fontSettingsWidget);

    m_widget->setLayout(m_mainlayout);
    setTitle(tr("Font"));
    setContent(m_widget);

    connect(m_fontSettingsWidget, &FontSettingsWidget::showStandardFont, this, &FontsWidget::showStandardFont);
    connect(m_fontSettingsWidget, &FontSettingsWidget::showMonoFont,    this, &FontsWidget::showMonoFont);
    connect(m_fontSizeWidget,     &FontSizeWidget::requestSetFontSize,  this, &FontsWidget::requestSetFontSize);
}

void FontsWidget::setModel(PersonalizationModel *const model)
{
    m_fontSizeWidget->setModel(model->getFontSizeModel());
    m_fontSettingsWidget->setFontStandModel(model->getStandFontModel());
    m_fontSettingsWidget->setFontMonoModel(model->getMonoFontModel());
}
