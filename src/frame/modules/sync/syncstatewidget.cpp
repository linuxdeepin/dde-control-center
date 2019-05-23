#include "syncstatewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::sync;
using namespace dcc::widgets;

SyncStateWidget::SyncStateWidget(QWidget *parent)
    : ContentWidget(parent)
    , m_syncIcon(new QLabel)
    , m_syncState(new QLabel)
    , m_lastSyncTime(new QLabel)
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_syncIcon, 0, Qt::AlignHCenter);
    layout->addWidget(m_syncState, 0, Qt::AlignHCenter);
    layout->addWidget(m_lastSyncTime, 0, Qt::AlignHCenter);
    layout->addSpacing(10);

    SettingsGroup* autoSyncGrp = new SettingsGroup;
    m_autoSync = new SwitchWidget(tr("Auto Sync"));
    autoSyncGrp->appendItem(m_autoSync);
    layout->addWidget(autoSyncGrp);
    layout->addSpacing(10);

    m_tips = new TipsLabel;
    m_tips->setWordWrap(true);
    m_tips->setText(tr("Switch it on to sync part of the settings for Control Center, Launcher and Dock automatically."));
    layout->addWidget(m_tips);
    layout->addSpacing(10);

    connect(m_autoSync, &SwitchWidget::checkedChanged, this, &SyncStateWidget::requestEnableSync);

    QMap<SyncModel::SyncType, QString> moduleTs{
        { SyncModel::SyncType::Network, tr("Network Settings") },
        { SyncModel::SyncType::Sound, tr("Sound Settings") },
        { SyncModel::SyncType::Mouse, tr("Mouse Settings") },
        { SyncModel::SyncType::Update, tr("Update Settings") },
        { SyncModel::SyncType::Dock, tr("Dock") },
        { SyncModel::SyncType::Launcher, tr("Launcher") },
        { SyncModel::SyncType::Wallpaper, tr("Wallpaper") }
    };

    m_moduleGrp = new SettingsGroup;
    QMap<SyncModel::SyncType, QString> moduleMap = m_model->moduleMap();
    for (auto it = moduleMap.cbegin(); it != moduleMap.cend(); ++it) {
        SwitchWidget* module = new SwitchWidget;
        module->setTitle(moduleTs[it.key()]);
        m_syncModuleMap[module] = it.key();
        m_moduleGrp->appendItem(module);
        connect(module, &SwitchWidget::checkedChanged, this, &SyncStateWidget::onModuleItemSwitched);
    }

    layout->addWidget(m_moduleGrp);

    layout->addStretch();

    TranslucentFrame* frame = new TranslucentFrame;
    frame->setLayout(layout);

    setContent(frame);

    m_syncState->hide();
    m_lastSyncTime->hide();
}

void SyncStateWidget::setModel(const SyncModel * const model)
{
    m_model = model;

    connect(model, &SyncModel::syncStateChanged, this, &SyncStateWidget::onStateChanged);
    connect(model, &SyncModel::lastSyncTimeChanged, this, &SyncStateWidget::onLastSyncTimeChanged);
    connect(model, &SyncModel::moduleSyncStateChanged, this, &SyncStateWidget::onModuleStateChanged);
    connect(model, &SyncModel::enableSyncChanged, this, &SyncStateWidget::onAutoSyncChanged);

    onStateChanged(model->syncState());
    onLastSyncTimeChanged(model->lastSyncTime());
    onAutoSyncChanged(model->enableSync());

    setTitle(model->userinfo()["Username"].toString());

    std::map<SyncModel::SyncType, bool> moduleState = m_model->moduleSyncState().toStdMap();
    for (auto it = moduleState.cbegin(); it != moduleState.cend(); ++it) {
        onModuleStateChanged(*it);
    };
}

void SyncStateWidget::onStateChanged(const std::pair<qint32, QString> &state)
{
    SyncModel::SyncState syncState;

    do {
        // check is sync succeed
        if (SyncModel::isSyncSucceed(state)) {
            syncState = SyncModel::SyncState::Succeed;
            break;
        }

        // check is syncing
        if (SyncModel::isSyncing(state)) {
            syncState = SyncModel::SyncState::Syncing;
            break;
        }

        // check is sync faild
        if (SyncModel::isSyncFailed(state)) {
            syncState = SyncModel::SyncState::Failed;
            break;
        }

        Q_UNREACHABLE();

    } while (false);

    switch (syncState) {
        case SyncModel::SyncState::Succeed:
            m_lastSyncTime->show();
            m_syncState->hide();
            break;
        case SyncModel::SyncState::Syncing:
            m_syncState->show();
            m_lastSyncTime->hide();
            break;
        case SyncModel::SyncState::Failed:
            break;
    }
}

void SyncStateWidget::onLastSyncTimeChanged(qlonglong lastSyncTime)
{
    m_lastSyncTime->setText(QString::number(lastSyncTime));
}

void SyncStateWidget::onModuleStateChanged(std::pair<SyncModel::SyncType, bool> state)
{
    SwitchWidget* widget = m_syncModuleMap.key(state.first);
    Q_ASSERT(widget);

    widget->setChecked(state.second);
}

void SyncStateWidget::onModuleItemSwitched(const bool checked)
{
    SwitchWidget* widget = qobject_cast<SwitchWidget*>(sender());
    Q_ASSERT(widget);

    Q_EMIT requestSetModuleState(std::pair<SyncModel::SyncType, bool>(m_syncModuleMap[widget], checked));
}

void SyncStateWidget::onAutoSyncChanged(bool autoSync)
{
    m_tips->setVisible(!autoSync);
    m_moduleGrp->setVisible(autoSync);
    m_autoSync->setChecked(autoSync);
}
