#include "networkdetailpage.h"
#include "networkmodel.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;

namespace dcc {

namespace network {

NetworkDetailPage::NetworkDetailPage(QWidget *parent)
    : ContentWidget(parent)
{
    m_groupsLayout = new QVBoxLayout;
    m_groupsLayout->setSpacing(10);
    m_groupsLayout->setMargin(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_groupsLayout);

    setTitle(tr("Network Details"));
    setContent(mainWidget);
}

void NetworkDetailPage::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::activeConnInfoChanged, this, &NetworkDetailPage::onActiveInfoChanged);

    onActiveInfoChanged(model->activeConnInfos());
}

void NetworkDetailPage::onActiveInfoChanged(const QList<QJsonObject> &infos)
{
    // clear old infos
    while (QLayoutItem *item = m_groupsLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    for (const auto &info : infos)
    {
        const QString name = info.value("ConnectionName").toString();
//        const QString speed = info.value("Speed").toString();

        SettingsGroup *grp = new SettingsGroup;
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(name);

        m_groupsLayout->addWidget(grp);
    }
}

}

}
