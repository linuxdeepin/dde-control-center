#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>

#include "interfaces.h"
#include "modulemetadata.h"

class QStackedLayout;
class QKeyEvent;
class HomeScreen;
class ContentView;
class Frame: public QFrame
{
    Q_OBJECT

public:
    Frame(QWidget * parent = 0);

    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;

private:
    HomeScreen * m_homeScreen;
    ContentView * m_contentView;
    QStackedLayout * m_stackedLayout;
    QList<ModuleMetaData> m_modules;

    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);
};

#endif
