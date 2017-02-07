#ifndef SYSTEMINFOMODEL_H
#define SYSTEMINFOMODEL_H

#include <QObject>

namespace dcc{
namespace systeminfo{

class SystemInfoModel : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoModel(QObject *parent = 0);

    void setEntryLists(const QStringList& list);
    QStringList entryLists() const { return m_entryLists;}

    QString defaultEntry() const { return m_defaultEntry;}
    QString distroid() const { return m_distroid;}
    QString distrover() const { return m_distrover;}
    QString version() const { return m_version;}
    QString type() const { return m_type;}
    QString processor() const { return m_processor;}
    QString memory() const { return m_memory;}
    QString disk() const { return m_disk;}

    bool bootDelay() const;
    bool themeEnabled() const { return m_themeEnabled; }
    bool updating() const { return m_updating; }

signals:
    void bootDelayChanged(const int timeout) const;
    void themeEnabledChanged(const bool enabled) const;
    void distroidChanged(const QString& distroid);
    void distroverChanged(const QString& distrover);
    void versionChanged(const QString& version);
    void typeChanged(const QString& type);
    void processorChanged(const QString& processor);
    void memoryChanged(const QString& memory);
    void diskChanged(const QString& disk);
    void defaultEntryChanged(const QString& entry);
    void updatingChanged(const bool &updating);

public slots:
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setDistroID(const QString& distroid);
    void setDistroVer(const QString& distrover);
    void setVersion(const QString& version);
    void setType(qlonglong type);
    void setProcessor(const QString& processor);
    void setMemory(qulonglong memory);
    void setDisk(qulonglong disk);
    void setDefaultEntry(const QString& entry);
    void setUpdating(bool updating);

private:
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;

    QString m_distroid;
    QString m_distrover;
    QString m_version;
    QString m_type;
    QString m_processor;
    QString m_memory;
    QString m_disk;
};

}
}
#endif // SYSTEMINFOMODEL_H
