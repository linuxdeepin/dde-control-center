#include "updatemodel.h"

namespace dcc{
namespace update{

DownloadInfo::DownloadInfo(const qlonglong &downloadSize, const QList<AppUpdateInfo> &appInfos, QObject *parent) :
    QObject(parent),
    m_downloadSize(downloadSize),
    m_downloadProgress(0),
    m_appInfos(appInfos)
{

}

void DownloadInfo::setDownloadProgress(double downloadProgress)
{
    if (m_downloadProgress != downloadProgress) {
        m_downloadProgress = downloadProgress;
        emit downloadProgressChanged(downloadProgress);
    }
}


UpdateModel::UpdateModel(QObject *parent) :
    QObject(parent),
    m_status(UpdatesStatus::Checking),
    m_downloadInfo(nullptr),
    m_autoUpdate(true),
    m_mirrorId("")
{

}

void UpdateModel::setMirrorInfos(const MirrorInfoList &list)
{
    m_mirrorList = list;
}

void UpdateModel::setDefaultMirror(const QString &mirror)
{
    if (mirror == m_mirrorId) return;
    m_mirrorId = mirror;

    QList<MirrorInfo>::iterator it = m_mirrorList.begin();
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == mirror)
        {
            emit defaultMirrorChanged(*it);
        }
    }
}

DownloadInfo *UpdateModel::downloadInfo() const
{
    return m_downloadInfo;
}

void UpdateModel::setDownloadInfo(DownloadInfo *downloadInfo)
{
    m_downloadInfo = downloadInfo;
}

MirrorInfo UpdateModel::defaultMirror() const
{
    QList<MirrorInfo>::const_iterator it = m_mirrorList.begin();
    for(; it != m_mirrorList.end(); ++it)
    {
        if((*it).m_id == m_mirrorId)
        {
            return *it;
        }
    }

    return m_mirrorList.at(0);
}

void UpdateModel::setAutoUpdate(bool update)
{
    if(m_autoUpdate == update)
        return;

    m_autoUpdate = update;
    emit autoUpdateChanged(m_autoUpdate);
}

UpdatesStatus UpdateModel::status() const
{
    return m_status;
}

void UpdateModel::setStatus(const UpdatesStatus &status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}


}
}
