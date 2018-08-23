#include "abstractsettings.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;

AbstractSettings::AbstractSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : QWidget(parent),
      m_connSettings(connSettings),
      m_sectionsLayout(new QVBoxLayout)
{
    m_sectionsLayout->setSpacing(10);
    m_sectionsLayout->setMargin(0);
    m_sectionsLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_sectionsLayout);
}

AbstractSettings::~AbstractSettings()
{
}

bool AbstractSettings::allInputValid()
{
    for (auto section : m_settingSections) {
        if (!section->allInputValid()) {
            qDebug() << "some section has Error!" << m_settingSections.indexOf(section);
            return false;
        }
    }

    return true;
}

void AbstractSettings::saveSettings()
{
    for (auto section : m_settingSections) {
        section->saveSettings();
    }
}
