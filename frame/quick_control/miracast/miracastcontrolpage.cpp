#include "miracastcontrolpage.h"
#include "miracastworker.h"
#include "basiclistview.h"
#include "miracastcontrolmodel.h"
#include "miracastcontroldelegate.h"

#include <com_deepin_daemon_miracast.h>

#include <QVBoxLayout>

using namespace dcc::widgets;

MiracastControlPage::MiracastControlPage(MiracastModel *model, QWidget *parent)
    : QWidget(parent),

      m_miracastModel(model)
{
    m_miracastWorker = new MiracastWorker(m_miracastModel);

    MiracastControlModel *ctrlModel = new MiracastControlModel(m_miracastModel);
    miracastControlDelegate *ctrlDelegate = new miracastControlDelegate;

    BasicListView *view = new BasicListView;
    view->setModel(ctrlModel);
    view->setItemDelegate(ctrlDelegate);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(view);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(view, &BasicListView::clicked, this, &MiracastControlPage::onItemClicked);
    connect(m_miracastModel, &MiracastModel::requestLinkScanning, m_miracastWorker, &MiracastWorker::setLinkScannning);
}

MiracastControlPage::~MiracastControlPage()
{
    m_miracastModel->deleteLater();
    m_miracastWorker->deleteLater();
}

void MiracastControlPage::onItemClicked(const QModelIndex &index)
{
    const MiracastInfo info = index.data(MiracastControlModel::MiracastItemInfoRole).value<MiracastInfo>();

    if (!info.m_sink)
    {
        qDebug() << *info.m_link;
        m_miracastWorker->setLinkEnable(info.m_link->m_dbusPath, !info.m_link->m_managed);
    } else {
        qDebug() << *info.m_sink;

        if (info.m_sink->m_connected)
            m_miracastWorker->disconnectSink(info.m_sink->m_sinkPath);
        else
            m_miracastWorker->connectSink(info.m_sink->m_sinkPath, QRect(0, 0, 1920, 1080));
    }
}
