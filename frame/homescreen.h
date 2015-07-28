#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "modulemetadata.h"

#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class QLabel;
class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QList<ModuleMetaData> modules, QWidget *parent = 0);

    void hide();
    void show();

signals:
    void moduleSelected(ModuleMetaData metaData);

private slots:
    void buttonClicked();

private:
    QGridLayout * m_grid;

    QGraphicsOpacityEffect *m_opacityEffect;

    int m_moduleCount = 0;
    const int m_animationDuration = 300;
};

class QEvent;
class QMouseEvent;
class ModuleButton : public QFrame
{
    Q_OBJECT
public:
    ModuleButton(ModuleMetaData metaData, QWidget * parent = 0);

    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    ModuleMetaData metaData();

signals:
    void clicked();

private:
    enum State { Normal, Hover };

private:
    void setState(State state);

private:
    ModuleMetaData m_meta;

    QLabel * m_icon;
    QLabel * m_text;
};

#endif // HOMESCREEN_H
