#ifndef FRAME_H
#define FRAME_H

#include <com_deepin_daemon_display.h>
#include <com_deepin_api_xmousearea.h>
#include <com_deepin_dde_launcher.h>

#include <QStack>
#include <QPropertyAnimation>

#include <DBlurEffectWidget>
#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>

#define BUTTON_LEFT 1
#define FRAME_WIDTH 360

using XMouseArea = com::deepin::api::XMouseArea;
using DBusDisplay = com::deepin::daemon::Display;
using LauncherInter = com::deepin::dde::Launcher;

DWIDGET_USE_NAMESPACE

class FrameWidget;
class SettingsWidget;

namespace dcc
{

class ContentWidget;

}

using dcc::ContentWidget;

class Frame : public DBlurEffectWidget
{
    Q_OBJECT

    friend class DBusControlCenterService;

public:
    explicit Frame(QWidget *parent = 0);

    void backToHome();
    void setDebugAutoHide(const bool autoHide);

public slots:
    void startup();

    void pushWidget(ContentWidget * const w);
    void popWidget();
    void showSettingsPage(const QString &moduleName, const QString &pageName);

signals:
    void rectChanged(const QRect &rect);

private slots:
    void init();
    void adjustShadowMask();
    void setAutoHide(const bool autoHide);

    void prepareAllSettingsPage();
    void freeAllSettingsPage();
    void showAllSettings();
    void contentDetached(QWidget * const c);

    void onScreenRectChanged(const QRect &primaryRect);
    void onMouseButtonReleased(const int button, const int x, const int y, const QString &key);

private:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
    void moveEvent(QMoveEvent *e);
    void show();
    void hide();
    void toggle();

private:
    QPointer<SettingsWidget> m_allSettingsPage;
    QTimer *m_allSettingsPageKiller;

    XMouseArea *m_mouseAreaInter;
    DBusDisplay *m_displayInter;
    LauncherInter *m_launcherInter;

    QString m_mouseAreaKey;
    QStack<FrameWidget *> m_frameWidgetStack;

    QRect m_primaryRect;
    QPropertyAnimation m_appearAnimation;

    DPlatformWindowHandle m_platformWindowHandle;
    DWindowManagerHelper *m_wmHelper;

    bool m_autoHide;
    bool m_debugAutoHide;
};

#endif // FRAME_H
