#include "generichotspotsection.h"

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;

GenericHotspotSection::GenericHotspotSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent),
      m_connIdItem(new LineEditWidget(this)),
      m_connSettings(connSettings)
{
    initUI();
}

GenericHotspotSection::~GenericHotspotSection()
{
}

bool GenericHotspotSection::allInputValid()
{
    bool valid = true;

    valid = !m_connIdItem->textEdit()->text().isEmpty();

    m_connIdItem->setIsErr(!valid);

    return valid;
}

void GenericHotspotSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());

    // hotspot connection should not be activated automatically
    m_connSettings->setAutoconnect(false);
}

void GenericHotspotSection::initUI()
{
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setText(m_connSettings->id());

    appendItem(m_connIdItem);

    connect(m_connIdItem->textEdit(), &QLineEdit::editingFinished, this, &GenericHotspotSection::allInputValid);
}
