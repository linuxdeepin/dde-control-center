#include "updatemodel.h"

namespace dcc{
namespace update{
UpdateModel::UpdateModel(QObject *parent) : QObject(parent)
{

}

void UpdateModel::setDefaultMirror(const QString &mirror)
{
    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == mirror)
        {
            m_mirror = (*it).m_name;
            break;
        }
    }
}

void UpdateModel::setMirrorInfoList(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

void UpdateModel::setAutoUpdate(bool update)
{
    if(m_autoUpdate == update)
        return;

    m_autoUpdate = update;
    emit autoUpdate(m_autoUpdate);
}

}
}
