#ifndef CONNECTIONSESSIONMODEL_H
#define CONNECTIONSESSIONMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>

namespace dcc {

namespace network {

class ConnectionSessionModel : public QObject
{
    Q_OBJECT

    friend class ConnectionSessionWorker;

public:
    explicit ConnectionSessionModel(QObject *parent = 0);

    const QStringList sections() const { return m_sections; }
    const QStringList sectionKeys(const QString &section) const { return m_visibleItems[section]; }
    const QString sectionName(const QString &section) const { return m_sectionName[section]; }
    const QMap<QString, QMap<QString, QJsonObject>> keys() const { return m_keys; }

signals:
    void visibleItemsChanged(const QMap<QString, QStringList> &keys) const;
    void keysChanged(const QMap<QString, QMap<QString, QJsonObject>> &keys) const;

private slots:
//    void setVisibleKeys(const QMap<QString, QStringList> &keys);
    void setAvailableItems(const QString &items);
    void setAllKeys(const QString &allKeys);

private:
    QList<QString> m_sections;
    QMap<QString, QString> m_sectionName;
    QMap<QString, QStringList> m_visibleItems;
    QMap<QString, QMap<QString, QJsonObject>> m_keys;
};

}

}

#endif // CONNECTIONSESSIONMODEL_H
