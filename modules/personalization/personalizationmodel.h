#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>


class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit PersonalizationModel(QObject *parent = 0);
    ~PersonalizationModel();

private:

};

#endif // PERSONALIZATIONMODEL_H
