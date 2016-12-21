#include "mirrorswidget.h"
#include "translucentframe.h"

#include <QStackedLayout>

#include "updatemodel.h"
#include "loadingitem.h"

namespace dcc{
namespace update{

MirrorsWidget::MirrorsWidget(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_curItem(nullptr),
      m_loadingGroup(new SettingsGroup),
      m_mirrorListGroup(new SettingsGroup),
      m_layout(new QStackedLayout)
{
    setTitle(tr("Switch Mirror"));

    LoadingItem *item = new LoadingItem;
    item->setMessage(tr("Checking speed, please wait"));
    m_loadingGroup->appendItem(item);

    TranslucentFrame* widget = new TranslucentFrame();

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_loadingGroup);
    m_layout->addWidget(m_mirrorListGroup);

    widget->setLayout(m_layout);
    setContent(widget);

    setModel(model);
}

void MirrorsWidget::setModel(UpdateModel *model)
{
    setDefaultMirror(model->defaultMirror());
    setMirrorInfoList(model->mirrorInfos());

    connect(model, &UpdateModel::defaultMirrorChanged, this, &MirrorsWidget::setDefaultMirror);
    connect(model, &UpdateModel::mirrorSpeedInfoAvaiable, this, &MirrorsWidget::onSpeedInfoAvailable);
}

void MirrorsWidget::setDefaultMirror(const MirrorInfo &mirror)
{
    if (mirror.m_id != m_defaultMirror.m_id) {
        m_defaultMirror = mirror;

        QList<MirrorItem*> items = findChildren<MirrorItem*>();
        for (MirrorItem *item : items) {
            if (item->mirrorInfo().m_id == m_defaultMirror.m_id) {
                blockSignals(true);
                setCurItem(item);
                blockSignals(false);
            }
        }
    }
}

void MirrorsWidget::setMirrorInfoList(const MirrorInfoList &list)
{
    QList<MirrorInfo>::const_iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        MirrorItem* item = new MirrorItem;
        item->setMirrorInfo((*it));

        if((*it).m_id == m_defaultMirror.m_id)
        {
            item->setSelected(true);
            m_curItem = item;
        }
        m_mirrorListGroup->appendItem(item);

        connect(item, SIGNAL(clicked(MirrorItem*)), this, SLOT(setCurItem(MirrorItem*)));
    }
}

void MirrorsWidget::setCurItem(MirrorItem *item)
{
    if(item)
    {
        if(m_curItem)
        {
            m_curItem->setSelected(false);
        }
        item->setSelected(true);
        m_curItem = item;

        MirrorInfo info = item->mirrorInfo();
        emit requestSetDefaultMirror(info);
    }
}

void MirrorsWidget::onSpeedInfoAvailable(const QMap<QString, int> &info)
{
    QList<MirrorItem*> items = findChildren<MirrorItem*>();
    for (MirrorItem *item : items) {
        const QString id = item->mirrorInfo().m_id;
        item->setSpeed(info.value(id, -1));
    }

    m_layout->setCurrentIndex(1);
}

}
}
