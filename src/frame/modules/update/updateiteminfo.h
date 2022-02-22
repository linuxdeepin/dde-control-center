#ifndef UPDATEITEMINFO_H
#define UPDATEITEMINFO_H


#include "common.h"
#include "widgets/utils.h"

namespace dcc {
namespace update {

struct DetailInfo {
    QString name;
    QString updateTime;
    QString info;
    QString link;

    DetailInfo() {}
};

class UpdateItemInfo: public QObject
{
    Q_OBJECT
public:

    explicit UpdateItemInfo(QObject *parent = nullptr);
    virtual ~UpdateItemInfo() {}

    inline qlonglong downloadSize() const   { return m_downloadSize; }
    void setDownloadSize(qlonglong downloadSize);

    double downloadProgress() const { return m_downloadProgress; }
    void setDownloadProgress(double downloadProgress);

    double installProgress() const { return m_installProgress; }
    void setInstallProgress(double installProgress);

    QString packageId() const;
    void setPackageId(const QString &packageId);

    QString name() const;
    void setName(const QString &name);

    QString currentVersion() const;
    void setCurrentVersion(const QString &currentVersion);

    QString availableVersion() const;
    void setAvailableVersion(const QString &availableVersion);

    QString explain() const;
    void setExplain(const QString &explain);

    QString updateTime() const;
    void setUpdateTime(const QString &updateTime);

    QList<DetailInfo> detailInfos() const;
    void setDetailInfos(QList<DetailInfo> &detailInfos);

Q_SIGNALS:
    void downloadProgressChanged(const double &progress);
    void installProgressChanged(const double &progress);
    void downloadSizeChanged(const qlonglong &size);

private:
    qlonglong m_downloadSize;
    double m_downloadProgress;
    double m_installProgress;

    QString m_packageId;
    QString m_name;
    QString m_currentVersion;
    QString m_availableVersion;
    QString m_explain;
    QString m_updateTime;
    QList<DetailInfo> m_detailInfos;
};

}
}

#endif // UPDATEITEMINFO_H
