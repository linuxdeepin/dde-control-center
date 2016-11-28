#include "fontsettingswidget.h"

FontSettingsWidget::FontSettingsWidget(QWidget *parent)
    :TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_fontWidget = new SettingsGroup;
    m_standard    = new NextPageWidget;
    m_mono       = new NextPageWidget;

    m_standard->setTitle(tr("Standard Font"));
    m_mono->setTitle(tr("Monospaced font"));

    m_fontWidget->appendItem(m_standard);
    m_fontWidget->appendItem(m_mono);

    m_mainlayout->addWidget(m_fontWidget);

    setLayout(m_mainlayout);
    setObjectName("FontSettingsWidget");

    connect(m_standard, &NextPageWidget::clicked, this, &FontSettingsWidget::showStandardFont);
    connect(m_mono, &NextPageWidget::clicked, this, &FontSettingsWidget::showMonoFont);
}

void FontSettingsWidget::setFontStandModel(FontModel *const model)
{
    m_standard->setValue(model->getFontName());
}

void FontSettingsWidget::setFontMonoModel(FontModel *const model)
{
    m_mono->setValue(model->getFontName());
}
