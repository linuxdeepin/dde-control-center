#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "pluginscontroller.h"
#include "framewidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <dimagebutton.h>
#include <dpageindicator.h>

class MainWidget : public FrameWidget
{
    Q_OBJECT

public:
    explicit MainWidget(Frame *parent = 0);

signals:
    void showAllSettings() const;

private:
    void showPlugin(QWidget * const w);

private slots:
    void pluginAdded(QWidget * const w);
    void showNextPlugin();

private:
    PluginsController *m_pluginsController;

    QWidget *m_lastPluginWidget;

    // ui widgets
    Dtk::Widget::DImageButton *m_userAvatarBtn;
    QLabel *m_currentTimeLbl;
    QLabel *m_currentDateLbl;
    QHBoxLayout *m_pluginsLayout;
    Dtk::Widget::DPageIndicator *m_pluginsIndicator;
    QPushButton *m_nextPluginBtn;
    QPushButton *m_allSettingsBtn;
};

#endif // MAINWIDGET_H
