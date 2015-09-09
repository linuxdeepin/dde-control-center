#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QAbstractButton>

#include "modulemetadata.h"
#include "dtipsframe.h"

class SideBarButton;
class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QList<ModuleMetaData> modules, QWidget *parent = 0);

    void switchToModule(const ModuleMetaData &meta);

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void switchToSideBarButton(SideBarButton *btn);

signals:
    void moduleSelected(ModuleMetaData meta);

private slots:
    void onSideBarButtonClicked();

private:
    SideBarButton *m_selectedBtn = NULL;
    DTipsFrame *m_tips;
};


class QLabel;
class QEvent;
class QMouseEvent;
class SideBarButton : public QAbstractButton
{
    Q_OBJECT

public:
    SideBarButton(ModuleMetaData metaData, QWidget *parent = 0);

    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

    void presse();
    void release();

    ModuleMetaData metaData();

signals:
    void clicked();
    void hovered();

private:
    enum State { Normal, Hover, Selected };

    ModuleMetaData m_meta;

    QLabel *m_icon;

    State m_state;

    void setState(State m_state);
};

#endif // SIDEBAR_H
