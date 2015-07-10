#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QFrame>

#include "modulemetadata.h"

class QLabel;
class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QList<ModuleMetaData> modules, QWidget *parent = 0);

signals:
    void moduleSelected(ModuleMetaData metaData);

private:
    QGridLayout * m_grid;

private slots:
    void buttonClicked();
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

    ModuleMetaData m_meta;

    QLabel * m_icon;
    QLabel * m_text;

    void setState(State state);
};

#endif // HOMESCREEN_H
