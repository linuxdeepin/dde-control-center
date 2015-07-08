#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QFrame>

#include "modulemetadata.h"

class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QList<ModuleMetaData> modules, QWidget *parent = 0);

private:
    QGridLayout * m_grid;
};

#endif // HOMESCREEN_H
