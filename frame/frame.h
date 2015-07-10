#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>

#include "interfaces.h"
#include "modulemetadata.h"

class QHBoxLayout;
class QKeyEvent;
class HomeScreen;
class Frame: public QFrame
{
    Q_OBJECT

public:
    Frame(QWidget * parent = 0);

    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;

private:
    QHBoxLayout * m_layout;
    HomeScreen * m_homeScreen;
    QList<ModuleMetaData> m_modules;

    void loadPlugin(QString path);
    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);
};

#endif
