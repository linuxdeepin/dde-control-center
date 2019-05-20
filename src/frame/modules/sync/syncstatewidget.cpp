#include "syncstatewidget.h"
#include "syncmodel.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::sync;
using namespace dcc::widgets;

SyncStateWidget::SyncStateWidget(QWidget *parent)
    : SettingsItem(parent)
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

    m_syncState->hide();
    m_lastSyncTime->hide();

    setLayout(layout);
}

void SyncStateWidget::setModel(const SyncModel * const model)
{
    m_model = model;

    connect(model, &SyncModel::syncStateChanged, this, &SyncStateWidget::onStateChanged);
    connect(model, &SyncModel::lastSyncTimeChanged, this, &SyncStateWidget::onLastSyncTimeChanged);

    onStateChanged(model->syncState());
    onLastSyncTimeChanged(model->lastSyncTime());
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
