#ifndef CATEGORY_H
#define CATEGORY_H
#include <QObject>
#include <QList>
#include <QJsonObject>
namespace dcc
{
namespace defapp
{
class Category : public QObject
{
    Q_OBJECT

public:
    explicit Category(QObject *parent = 0);

    void setappList(const QList<QJsonObject> &list);   //设置list到暂存区
    void setuserList(const QList<QJsonObject> &list);
    void setDefault(const QJsonObject &def);

    const QString getName() const { return m_category;}
    void setCategory(const QString &category);
    inline const QList<QJsonObject> getappItem() const { return m_applist;}
    inline const QList<QJsonObject> getuserItem() const { return m_userlist;}
    inline const QJsonObject getDefault() { return m_default;}
    void addUserItem(const QJsonObject &value);
    void delUserItem(const QJsonObject &value);

signals:
    void itemsChanged(const QList<QJsonObject> &list);
    void userItemChanged(const QList<QJsonObject> &list);
    void defaultChanged(const QJsonObject &id);
    void AdduserItem(const QJsonObject &json);
    void categoryNameChanged(const QString &name);

private:
    QList<QJsonObject> m_applist;
    QList<QJsonObject> m_userlist;
    QString m_category;
    QJsonObject m_default;
};
}
}

#endif // CATEGORY_H
