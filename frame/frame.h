#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>

#include "interfaces.h"
#include "modulemetadata.h"

class HomeScreen;
class Frame: public QFrame
{
    Q_OBJECT

public:
    Frame(QWidget * parent = 0);

private:
    HomeScreen * m_homeScreen;
    QList<ModuleMetaData> m_modules;

    void loadPlugins();
};

#endif
