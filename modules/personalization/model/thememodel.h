#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QDebug>

namespace dcc
{
namespace personalization
{
class ThemeModel : public QObject
{
    Q_OBJECT
public:
    explicit ThemeModel(QObject *parent = 0);

    void addItem(const QString &id, const QJsonObject &json);
    QMap<QString, QJsonObject> getList() {return m_list;}

    void setDefault(const QString &value);
    inline QString getDefault() {return m_default;}

    QMap<QString, QString> getPicList() const;
    void addPic(const QString &id, const QString &picPath);

    void removeItem(const QString &id);

signals:
    void itemAdded(const QJsonObject &json);
    void defaultChanged(const QString &value);
    void picAdded(const QString &id, const QString &picPath);
    void itemRemoved(const QString &id);

private:
    QMap<QString, QJsonObject> m_list;
    QString m_default;
    QMap<QString, QString> m_picList;
};
}
}


#endif // THEMEMODEL_H
