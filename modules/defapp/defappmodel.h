#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include <QObject>
#include <QList>
#include "optionwidget.h"
#include <QDebug>
#include <vector>
#include <iterator>

class DefAppWorker;


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

signals:
    void itemsChanged();
    void userItemChanged();
    void defaultChanged(const QString &id);

private:
    QList<QJsonObject> m_applist; //暂存list
    QList<QJsonObject> m_userlist;
    QString m_category;
    QString m_id;
};

class DefAppModel : public QObject
{
    Q_OBJECT

    friend class DefAppWorker;

public:
    explicit DefAppModel(QObject *parent = 0);
    ~DefAppModel();
    inline bool setAutoOpen() {return m_autoOpen;}
    Category *getCategory(const QString &category);

signals:
    void AutoOpenChanged(bool state);

private slots:
    void setAppList(const QString &category, QList<QJsonObject> &list); //该函数遍历对象来设置list
    void setUserList(const QString &category, QList<QJsonObject> &list);
    void setDefault(const QString &category, const QString &id);
    void setAutoOpen(const bool state);


private:
    bool m_autoOpen;
    QList<Category *> m_categoryList;
};

#endif // DEFAPPMODEL_H
