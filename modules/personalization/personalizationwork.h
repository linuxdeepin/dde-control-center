#ifndef PERSONALIZATIONWORK_H
#define PERSONALIZATIONWORK_H

#include "personalizationmodel.h"
#include "personalizationmodel.h"

#include <QObject>

class PersonalizationWork : public QObject
{
    Q_OBJECT
public:
    PersonalizationWork(PersonalizationModel *model, QObject *parent = 0);
    void active();
    void deactive();

private:
    PersonalizationModel *m_model;
};

#endif // PERSONALIZATIONWORK_H
