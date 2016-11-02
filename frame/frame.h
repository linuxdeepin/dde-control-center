#ifndef FRAME_H
#define FRAME_H

#include "mainwidget.h"
#include "contentwidget.h"
#include "settingswidget.h"
#include "frameproxyinterface.h"

#include <com_deepin_daemon_display.h>

#include <QMainWindow>
#include <QStack>

using Display = com::deepin::daemon::Display;

class Frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);

public slots:
    void startup();

    void pushWidget(ContentWidget * const w);
    void popWidget();

private slots:
    void init();

    void showAllSettings();
    void contentDetached(QWidget * const c);

    void onScreenRectChanged(const QRect &primaryRect);

private:
    void keyPressEvent(QKeyEvent *e);
    void hide();

private:
    SettingsWidget *m_allSettingsPage;

    Display *m_displayInter;

    QStack<FrameWidget *> m_frameWidgetStack;
};

#endif // FRAME_H
