#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>
#include <QGraphicsOpacityEffect>

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
    inline void switchToHome();
    inline void switchToContent();

    void listPlugins();

private slots:
    void selectModule(ModuleMetaData metaData);

private:
    HomeScreen * m_homeScreen;
    ContentView * m_contentView;
    QList<ModuleMetaData> m_modules;

    QGraphicsOpacityEffect *m_homeEffect;
    QGraphicsOpacityEffect *m_contentEffect;

    const int animationDuration = 300;
};

#endif
