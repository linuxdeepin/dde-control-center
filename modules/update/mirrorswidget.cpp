#include "mirrorswidget.h"
#include "translucentframe.h"

#include <QVBoxLayout>

#include "updatemodel.h"

namespace dcc{
namespace update{

MirrorsWidget::MirrorsWidget(UpdateModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_curItem(nullptr)
{
    setTitle(tr("Switch Mirror"));

    TranslucentFrame* widget = new TranslucentFrame();

    QVBoxLayout* layout = new QVBoxLayout;
    m_group = new SettingsGroup();
    layout->addWidget(m_group);

    widget->setLayout(layout);
    setContent(widget);

    setModel(model);
}

void MirrorsWidget::setModel(UpdateModel *model)
{
    setDefaultMirror(model->defaultMirror());
    setMirrorInfoList(model->mirrorInfos());

    connect(model, &UpdateModel::defaultMirrorChanged, this, &MirrorsWidget::setDefaultMirror);
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
        MirrorItem* item = new MirrorItem();
        item->setMirrorInfo((*it));

        if((*it).m_id == m_defaultMirror.m_id)
        {
            item->setSelected(true);
            m_curItem = item;
        }
        m_group->appendItem(item);

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

}
}
