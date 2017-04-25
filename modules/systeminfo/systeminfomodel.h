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

#ifndef DCC_DISABLE_GRUB
    void setEntryLists(const QStringList& list);
    QStringList entryLists() const { return m_entryLists;}
    QString defaultEntry() const { return m_defaultEntry;}
#endif

    QString distroid() const { return m_distroid;}
    QString distrover() const { return m_distrover;}
    QString version() const { return m_version;}
    QString type() const { return m_type;}
    QString processor() const { return m_processor;}
    QString memory() const { return m_memory;}
    QString disk() const { return m_disk;}

#ifndef DCC_DISABLE_GRUB
    bool bootDelay() const;
    bool themeEnabled() const { return m_themeEnabled; }
    bool updating() const { return m_updating; }
#endif

signals:
#ifndef DCC_DISABLE_GRUB
    void bootDelayChanged(const int timeout) const;
    void themeEnabledChanged(const bool enabled) const;
    void entryListsChanged(const QStringList &list);
    void defaultEntryChanged(const QString& entry);
    void updatingChanged(const bool &updating);
#endif
    void distroidChanged(const QString& distroid);
    void distroverChanged(const QString& distrover);
    void versionChanged(const QString& version);
    void typeChanged(const QString& type);
    void processorChanged(const QString& processor);
    void memoryChanged(const QString& memory);
    void diskChanged(const QString& disk);

public slots:
#ifndef DCC_DISABLE_GRUB
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setDefaultEntry(const QString& entry);
    void setUpdating(bool updating);
#endif
    void setDistroID(const QString& distroid);
    void setDistroVer(const QString& distrover);
    void setVersion(const QString& version);
    void setType(qlonglong type);
    void setProcessor(const QString& processor);
    void setMemory(qulonglong memory);
    void setDisk(qulonglong disk);

private:
#ifndef DCC_DISABLE_GRUB
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;
#endif

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
