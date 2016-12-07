#include "mirrorswidget.h"
#include "translucentframe.h"
#include <QVBoxLayout>

MirrorsWidget::MirrorsWidget(QWidget *parent)
    :ContentWidget(parent),
      m_curItem(nullptr)
{
    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();

    m_group = new SettingsGroup();

    layout->addWidget(m_group);
    widget->setLayout(layout);
    setContent(widget);
}

void MirrorsWidget::setDefaultMirror(const QString &mirror)
{
    m_defaultMirror = mirror;
}

void MirrorsWidget::setMirrorInfoList(const MirrorInfoList &list)
{
    QList<MirrorInfo>::const_iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        MirrorItem* item = new MirrorItem();
        item->setMirrorInfo((*it));
        if((*it).m_id == m_defaultMirror)
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
        emit mirrorName(info.m_name, info.m_id);
    }
}
