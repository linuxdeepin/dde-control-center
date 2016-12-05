#include "basicsettingspage.h"

#include <QVBoxLayout>

using namespace dcc;

BasicSettingsPage::BasicSettingsPage(QWidget *parent)
    : QWidget(parent),

      m_soundSlider(new DCCSlider),
      m_lightSlider(new DCCSlider)
{
    m_soundSlider->setOrientation(Qt::Horizontal);
    m_lightSlider->setOrientation(Qt::Horizontal);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_soundSlider);
    mainLayout->addWidget(m_lightSlider);

    setLayout(mainLayout);
}
