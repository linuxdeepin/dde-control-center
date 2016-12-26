#ifndef FRAME_H
#define FRAME_H

#include "mainwidget.h"
#include "contentwidget.h"
#include "settingswidget.h"
#include "frameproxyinterface.h"

#include <com_deepin_daemon_display.h>
#include <com_deepin_api_xmousearea.h>

#include <QStack>
#include <QPropertyAnimation>

#include "blurredframe.h"

using XMouseArea = com::deepin::api::XMouseArea;
using DBusDisplay = com::deepin::daemon::Display;

class Frame : public BlurredFrame
{
    Q_OBJECT

    friend class DBusControlCenterService;

public:
    explicit Frame(QWidget *parent = 0);

public slots:
    void startup();

    void pushWidget(ContentWidget * const w);
    void popWidget();

private slots:
    void init();
    void setAutoHide(const bool autoHide);

    void showAllSettings();
    void showSettingsPage(const QString &moduleName, const QString &pageName);
    void contentDetached(QWidget * const c);

    void onScreenRectChanged(const QRect &primaryRect);
    void onMouseButtonReleased(const int button, const int x, const int y, const QString &key);

private:
    void keyPressEvent(QKeyEvent *e);
    void show();
    void hide();
    void toggle();

private:
    SettingsWidget *m_allSettingsPage;

    XMouseArea *m_mouseAreaInter;
    DBusDisplay *m_displayInter;

    QString m_mouseAreaKey;
    QStack<FrameWidget *> m_frameWidgetStack;

    QRect m_primaryRect;
    QPropertyAnimation m_appearAnimation;

    bool m_autoHide;
};

#endif // FRAME_H
