#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "modulemetadata.h"
#include "pluginsmanager.h"

#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QLabel;
class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = 0);
    ~HomeScreen();

    void hide();
    void show();

signals:
    void moduleSelected(const QString pluginId) const;
    void showAniFinished();
    void powerBtnClicked();

public slots:
    void powerButtonClicked();

private:
    QGridLayout *m_grid;

    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_topAni;
    QPropertyAnimation *m_ctrShowAni;
    QPropertyAnimation *m_ctrHideAni;
    QPropertyAnimation *m_botAni;

    QWidget *m_topWidget;
    QWidget *m_centerWidget;
    QWidget *m_bottomWidget;

    PluginsManager *m_pluginsManager;

    int m_moduleCount = 0;
};

class QEvent;
class QMouseEvent;
class ModuleButton : public QFrame
{
    Q_OBJECT
public:
    ModuleButton(const ModuleMetaData &metaData, QWidget *parent = 0);

    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

signals:
    void clicked(const QString pluginId) const;

private:
    enum State { Normal, Hover };

private:
    void setState(State state);

private:
    QString m_pluginId;

    QLabel *m_icon;
    QLabel *m_text;
};

#endif // HOMESCREEN_H
