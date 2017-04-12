#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "pluginscontroller.h"
#include "framewidget.h"
#include "indicatorwidget.h"
#include "quick_control/quickcontrolpanel.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <dimagebutton.h>
#include <dpageindicator.h>

namespace dcc {
namespace accounts {
    class AvatarWidget;
}
}

class UpdateNotifier;
class MainWidget : public FrameWidget
{
    Q_OBJECT

public:
    explicit MainWidget(Frame *parent = 0);

signals:
    void showAllSettings() const;
    void showSettingPage(const QString &module, const QString &page);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    int getPluginsHeight();
    void updatePluginsHeight();

private slots:
    void pluginAdded(QWidget * const w);
    void showNextPlugin();
    void showPrevPlugin();
    void refershTimedate();

private:
    PluginsController *m_pluginsController;

//    QWidget *m_lastPluginWidget;
    QTimer *m_timeRefersh;

    // ui widgets
    dcc::accounts::AvatarWidget *m_userAvatarBtn;
    QLabel *m_currentTimeLbl;
    QLabel *m_currentDateLbl;
//    QHBoxLayout *m_pluginsLayout;
    QStackedLayout *m_pluginsLayout;
    QWidget *m_pluginWrapper;
    QWidget *m_pluginWidget;
    IndicatorWidget *m_indicatorWidget;
#ifndef DISABLE_SYS_UPDATE
    UpdateNotifier *m_updateNotifier;
#endif
    QuickControlPanel *m_quickSettingsPanel;
};

#endif // MAINWIDGET_H
