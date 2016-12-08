#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QDebug>
class Category : public QObject
{
    Q_OBJECT

public:
    explicit Category(QObject *parent = 0);

    void setappList(const QList<QJsonObject> &list);   //设置list到暂存区
    void setuserList(const QList<QJsonObject> &list);
    void setDefault(const QString &id);

    inline const QString getName() const { return m_category;}
    inline void setCategory(const QString &category) { m_category = category;}
    inline const QList<QJsonObject> getappItem() const { return m_applist;}
    inline const QList<QJsonObject> getuserItem() const { return m_userlist;}
    inline const QString getDefault() { return m_id;}
    void addUserItem(const QJsonObject &value);
    void delUserItem(const QJsonObject &value);

signals:
    void itemsChanged(const QList<QJsonObject> &list);
    void userItemChanged(const QList<QJsonObject> &list);
    void defaultChanged(const QString &id);
    void AdduserItem(const QJsonObject &json);

private:
    QList<QJsonObject> m_applist;
    QList<QJsonObject> m_userlist;
    QString m_category;
    QString m_id;
};

#endif // CATEGORY_H
