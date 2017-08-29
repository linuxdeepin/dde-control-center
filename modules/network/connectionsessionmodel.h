#ifndef CONNECTIONSESSIONMODEL_H
#define CONNECTIONSESSIONMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>

#include <types/networkerrors.h>

namespace dcc {

namespace network {

class ConnectionSessionModel : public QObject
{
    Q_OBJECT

    friend class ConnectionSessionWorker;

public:
    explicit ConnectionSessionModel(QObject *parent = 0);

    bool deletable() const { return m_deletable; }
    const QString type() const { return m_type; }
    const QString uuid() const { return m_uuid; }
    const NetworkErrors errors() const { return m_errors; }
    const QStringList sections() const { return m_sections; }
    const QList<QJsonObject> sectionKeys(const QString &section) const { return m_visibleItems[section]; }
    const QJsonObject keysInfo(const QString &section, const QString &vKey) const;
    const QString sectionName(const QString &section) const { return m_sectionName[section]; }
    const QString virtualSectionName(const QString &realSectionName) const { return m_virtualSectionName[realSectionName]; }
    const QMap<QString, QMap<QString, QJsonObject>> vkList() const { return m_vks; }
    const QJsonObject vkInfo(const QString &section, const QString &vKey) const { return m_vks[section][vKey]; }

signals:
    void saveFinished(const bool ret) const;
    void deletableChanged(const bool deletable) const;
    void typeChanged(const QString &type) const;
    void uuidChanged(const QString &conn) const;
    void visibleItemsChanged(const QMap<QString, QList<QJsonObject>> &vkList) const;
    void keysChanged(const QMap<QString, QMap<QString, QJsonObject>> &vkList) const;
    void errorsChanged(const NetworkErrors &errors) const;

private slots:
//    void setVisibleKeys(const QMap<QString, QStringList> &keys);
    void setDeletable(const bool deletable);
    void setType(const QString &type);
    void setConnUuid(const QString &connUuid);
    void setErrors(const NetworkErrors &errors);
    void setAvailableItems(const QString &items);
    void setAllKeys(const QString &allKeys);

private:
    bool m_deletable;
    QString m_uuid;
    QString m_type;
    QList<QString> m_sections;
    NetworkErrors m_errors;
    QMap<QString, QString> m_sectionName;
    QMap<QString, QString> m_virtualSectionName;
    QMap<QString, QList<QJsonObject>> m_visibleItems;
    QMap<QString, QMap<QString, QJsonObject>> m_vks;
};

}

}

#endif // CONNECTIONSESSIONMODEL_H
