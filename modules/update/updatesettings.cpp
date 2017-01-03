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

    m_autoDownloadSwitch = new SwitchWidget;
    m_autoDownloadSwitch->setTitle(tr("Auto-download Updates"));

    SmallLabel* label = new SmallLabel(tr("Updates will be auto-downloaded in wireless or wired network"));
    label->setWordWrap(true);

    m_updateMirrors = new NextPageWidget;
    m_updateMirrors->setTitle(tr("Switch Mirror"));

    ug->appendItem(m_autoDownloadSwitch);
    mg->appendItem(m_updateMirrors);

    layout->addWidget(ug);
    layout->addWidget(label);
    layout->addSpacing(10);
    layout->addWidget(mg);
    layout->addStretch();

    widget->setLayout(layout);

    setContent(widget);

    connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
    connect(m_autoDownloadSwitch, &SwitchWidget::checkedChanegd, this, &UpdateSettings::requestSetAutoUpdate);

    setModel(model);
}

void UpdateSettings::setModel(UpdateModel *model)
{
    if (model) {
        m_model = model;

        auto setAutoDownload = [this] (const bool &autoDownload) {
            m_autoDownloadSwitch->setChecked(autoDownload);
        };

        auto setDefaultMirror = [this] (const MirrorInfo &mirror) {
            m_updateMirrors->setValue(mirror.m_name);
        };

        setAutoDownload(model->autoDownloadUpdates());
        setDefaultMirror(model->defaultMirror());

        connect(model, &UpdateModel::autoDownloadUpdatesChanged, this, setAutoDownload);
        connect(model, &UpdateModel::defaultMirrorChanged, this, setDefaultMirror);
    }
}

}
}
