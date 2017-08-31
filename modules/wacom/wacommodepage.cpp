#include "wacommodepage.h"
#include "settingsgroup.h"
#include "optionitem.h"
#include "translucentframe.h"

using namespace dcc;
using namespace dcc::wacom;
using namespace dcc::widgets;

WacomModePage::WacomModePage(QWidget *parent) : ContentWidget(parent)
{
    m_penMode = new OptionItem(tr("Pen"), false);
    m_mouseMode = new OptionItem(tr("Mouse"), false);

    m_penMode->setContentsMargins(20, 0, 10, 0);
    m_mouseMode->setContentsMargins(20, 0, 10, 0);

    SettingsGroup *grp = new SettingsGroup;
    grp->appendItem(m_penMode);
    grp->appendItem(m_mouseMode);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    vlayout->addSpacing(10);
    vlayout->addWidget(grp);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(vlayout);

    setContent(w);

    connect(m_penMode, &OptionItem::selectedChanged, this, &WacomModePage::modeSelected);
    connect(m_mouseMode, &OptionItem::selectedChanged, this, &WacomModePage::modeSelected);
}

void WacomModePage::setMode(const bool mode)
{
    m_penMode->blockSignals(true);
    m_mouseMode->blockSignals(true);

    m_penMode->setSelected(!mode);
    m_mouseMode->setSelected(mode);

    m_penMode->blockSignals(false);
    m_mouseMode->blockSignals(false);
}

void WacomModePage::modeSelected()
{
    OptionItem *item = qobject_cast<OptionItem*>(sender());

    Q_ASSERT(item);

    const bool mode = (item == m_mouseMode);

    setMode(mode);

    emit requestSetMode(mode);
    emit back();
}
