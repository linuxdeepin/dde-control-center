#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>


class DefAppWorker;
class DefAppModel : public QObject
{
    Q_OBJECT

    friend class DefAppWorker;

public:
    explicit DefAppModel(QObject *parent = 0);
    inline QMultiMap<QString,QStringList> setList() {return m_appsList;}
    inline QMultiMap<QString,QString> setDefApp() {return m_defapp;}

signals:
    void ListAppChanged(QMap<QString,QStringList> List);  //列表改变信号
    void DefaultAppsChanged(QMap<QString,QString> app); //默认程序改变信号

private:
    void setList(QMultiMap<QString,QStringList> List);
    void setDefApp(QMultiMap<QString,QString> app);

    QMultiMap<QString,QStringList> m_appsList;
    QMultiMap<QString,QString> m_defapp;
};
#endif // DEFAPPMODEL_H
