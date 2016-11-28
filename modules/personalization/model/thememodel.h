#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QDebug>
class ThemeModel : public QObject
{
    Q_OBJECT
public:
    explicit ThemeModel(QObject *parent = 0);
    void setJson(const QList<QJsonObject> &json);
    QList<QJsonObject> getJson() {return m_list;}
    void setDefault(const QString &value);
    inline QString getDefault() {return m_default;}

signals:
    void listChanged(const QList<QJsonObject> &json);
    void defaultChanged(const QString &value);

private:
    QList<QJsonObject> m_list;
    QString m_default;
};

#endif // THEMEMODEL_H
