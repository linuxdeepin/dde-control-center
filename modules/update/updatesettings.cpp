#include "updatesettings.h"

#include <QVBoxLayout>

#include "settingsgroup.h"
#include "updatemodel.h"
#include "translucentframe.h"
#include "labels/smalllabel.h"

namespace dcc{
namespace update{

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_model(nullptr)
{
    setTitle(tr("Update Settings"));

    TranslucentFrame* widget = new TranslucentFrame;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    SettingsGroup* ug = new SettingsGroup;
    SettingsGroup* mg = new SettingsGroup;

    m_autoUpdate = new SwitchWidget;
    m_autoUpdate->setTitle(tr("Auto-­download Updates"));

    SmallLabel* label = new SmallLabel(tr("Updates will be auto-­downloaded in wireless or wired network"));
    label->setWordWrap(true);

    m_updateMirrors = new NextPageWidget;
    m_updateMirrors->setTitle(tr("Switch Mirror"));

    ug->appendItem(m_autoUpdate);
    mg->appendItem(m_updateMirrors);

    layout->addWidget(ug);
    layout->addWidget(label);
    layout->addSpacing(10);
    layout->addWidget(mg);

    widget->setLayout(layout);

    setContent(widget);

    connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
    connect(m_autoUpdate, &SwitchWidget::checkedChanegd, this, &UpdateSettings::requestSetAutoUpdate);

    setModel(model);
}

void UpdateSettings::setModel(UpdateModel *model)
{
    m_model = model;

    auto setAutoUpdate = [this] (const bool &autoUpdate) {
        m_autoUpdate->blockSignals(true);
        m_autoUpdate->setChecked(autoUpdate);
        m_autoUpdate->blockSignals(false);
    };

    auto setDefaultMirror = [this] (const MirrorInfo &mirror) {
        m_updateMirrors->setValue(mirror.m_name);
    };

    setAutoUpdate(model->autoUpdate());
    setDefaultMirror(model->defaultMirror());

    connect(model, &UpdateModel::autoUpdateChanged, setAutoUpdate);
    connect(model, &UpdateModel::defaultMirrorChanged, setDefaultMirror);
}

}
}
