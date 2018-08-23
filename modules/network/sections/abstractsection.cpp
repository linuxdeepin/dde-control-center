#include "abstractsection.h"

using namespace dcc::widgets;

AbstractSection::AbstractSection(QFrame *parent)
    : SettingsGroup(parent)
{
}

AbstractSection::AbstractSection(const QString &title, QFrame *parent)
    : SettingsGroup(title, parent)
{
}

AbstractSection::~AbstractSection()
{
}

void AbstractSection::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    allInputValid();
}
