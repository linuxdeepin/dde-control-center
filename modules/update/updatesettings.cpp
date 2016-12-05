#include "updatesettings.h"
#include "settingsgroup.h"
#include "updatemodel.h"
#include "updateitem.h"

#include <QVBoxLayout>

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_model(model)
{
    QWidget* widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    SettingsGroup* ug = new SettingsGroup();
    SettingsGroup* mg = new SettingsGroup();

    m_autoUpdate = new SwitchWidget();
    m_autoUpdate->setTitle(tr("Auto-­download Updates"));

    QLabel* label = new QLabel();
    label->setWordWrap(true);
    label->setText(tr("Updates will be auto-­downloaded in wireless or wired network"));
    m_updateMirrors = new NextPageWidget();
    m_updateMirrors->setTitle(tr("Switch Mirror"));

    ug->appendItem(m_autoUpdate);
    mg->appendItem(m_updateMirrors);

    layout->addWidget(ug);
    layout->addWidget(label);
    layout->addWidget(mg);

    widget->setLayout(layout);

    setContent(widget);

    connect(m_updateMirrors, SIGNAL(clicked()), this, SIGNAL(mirrors()));
    connect(m_autoUpdate,SIGNAL(checkedChanegd(bool)), this, SIGNAL(autoUpdate(bool)));
    connect(m_model, SIGNAL(autoUpdate(bool)), this, SLOT(setAutoUpdate(bool)));
}

void UpdateSettings::setDefaultMirror(const QString &value)
{
    m_updateMirrors->setValue(value);
}

void UpdateSettings::setAutoUpdate(bool update)
{
    m_autoUpdate->setChecked(update);
}
