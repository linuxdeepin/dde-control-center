#ifndef PERSONALIZATIONWORK_H
#define PERSONALIZATIONWORK_H

#include "personalizationmodel.h"

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <com_deepin_daemon_apperance.h>
using com::deepin::daemon::Appearance;

class PersonalizationWork : public QObject
{
    Q_OBJECT
public:
    PersonalizationWork(PersonalizationModel *model, QObject *parent = 0);
    void active();
    void deactive();

public slots:
    void setDefault(const QJsonObject &value);
    void setFontSize(const int value);
    void setList(const QString &type, const QString &key);

private slots:
    void FontSizeChanged(const int  value) const;

private:
    int sizeToSliderValue(const int value) const;
    int sliderValueToSize(const int value) const;
    bool isFont(const QString &type);
    QString getThumbnail(const QString &Type, const QString &key);
    QList<QJsonObject> converToList(const QString &type, QJsonArray &array);

private:
    PersonalizationModel *m_model;
    Appearance           *m_dbus;
};

#endif // PERSONALIZATIONWORK_H
