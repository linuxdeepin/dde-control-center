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

    const QStringList sections() const { return m_sections; }
    const QList<QJsonObject> sectionKeys(const QString &section) const { return m_visibleItems[section]; }
    const QJsonObject keysInfo(const QString &section, const QString &vKey) const;
    const QString sectionName(const QString &section) const { return m_sectionName[section]; }
    const QMap<QString, QMap<QString, QJsonObject>> keys() const { return m_keys; }

signals:
    void visibleItemsChanged(const QMap<QString, QList<QJsonObject>> &keys) const;
    void keysChanged(const QMap<QString, QMap<QString, QJsonObject>> &keys) const;
    void errorsChanged(const NetworkErrors &errors) const;

private slots:
//    void setVisibleKeys(const QMap<QString, QStringList> &keys);
    void setAvailableItems(const QString &items);
    void setAllKeys(const QString &allKeys);
    void onErrorsChanged(const NetworkErrors &errors) const;

private:
    QList<QString> m_sections;
    QMap<QString, QString> m_sectionName;
    QMap<QString, QList<QJsonObject>> m_visibleItems;
    QMap<QString, QMap<QString, QJsonObject>> m_keys;
};

}

}

#endif // CONNECTIONSESSIONMODEL_H
